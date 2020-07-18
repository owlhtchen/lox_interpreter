
#include "CallFrame.h"
#include <DerivedObject.h>
#include <VM.h>
#include <util.h>
#include <Value.h>
#include <iostream>
#include <stdexcept>
#include <GarbageCollector.h>
#include <stdexcept>
#include <debug.h>

void CallFrame::runFrame() {
    OpCode currentOpcode;
    Value second, first;
    auto& chunk = getCurrentChunk();
//    std::cerr << " in call frame of " << closureObj->toString() << " on line: " << getCurrentLine() << std::endl;

    while(ip < chunk.code.size()) {
        currentOpcode = readOpCode();
        switch (currentOpcode) {
            case OpCode::OP_CONSTANT: {
                pushStack(readConstant());
                break;
            }
            case OpCode::OP_TRUE: {
                pushStack(true);
                break;
            }
            case OpCode::OP_FALSE: {
                pushStack(false);
                break;
            }
            case OpCode::OP_NIL: {
                pushStack(std::monostate());
                break;
            }
            case OpCode::OP_NEGATE: {
                first = popStack();
                if (auto first_double = std::get_if<double>(&first)) {
                    pushStack(-*first_double);
                } else {
                    throw RuntimeError(getCurrentLine(), "negate opr can only be used with a number");
                }
                break;
            }
            case OpCode::OP_NOT: {
                first = popStack();
                if (auto first_double = std::get_if<bool>(&first)) {
                    pushStack(!*first_double);
                } else {
                    throw RuntimeError(getCurrentLine(), "not opr can only be used with a boolean value");
                }
                break;
            }
            case OpCode::OP_ADD:
            case OpCode::OP_SUBTRACT:
            case OpCode::OP_MULTIPLY:
            case OpCode::OP_DIVIDE: {
                second = popStack();
                first = popStack();
                auto second_double = std::get_if<double>(&second);
                auto first_double = std::get_if<double>(&first);
                if(second_double && first_double) {
                    pushStack(basic_arithmetic(currentOpcode, *first_double, *second_double));
                } else if(currentOpcode == OpCode::OP_ADD) {
                    Object* second_obj = *std::get_if<Object*>(&second);
                    Object* first_obj = *std::get_if<Object*>(&first);
                    if(second_obj && first_obj) {
                        auto second_str = second_obj->dyn_cast<StringObj>();
                        auto first_str = first_obj->dyn_cast<StringObj>();
                        std::string new_str = first_str->str + second_str->str;
                        pushStack(StringPool::getInstance().getStringObj(new_str));
//                        stack.push_back(first_obj->str + second_obj->str);
                    }
                } else {
                    throw RuntimeError(getCurrentLine(), "+ - * / opr only support number or string (for add)");
                }
                break;
            }
            case OpCode::OP_EQUAL: {
                second = popStack();
                first = popStack();
                auto second_double = std::get_if<double>(&second);
                auto first_double = std::get_if<double>(&first);
                if(second_double && first_double) {
                    pushStack(basic_boolean(currentOpcode, *first_double, *second_double));
                    break;
                }
                auto second_bool = std::get_if<bool>(&second);
                auto first_bool = std::get_if<bool>(&first);
                if(second_bool && first_bool) {
                    pushStack(*first_bool == *second_bool);
                    break;
                }
                auto second_obj = *std::get_if<Object*>(&second);
                auto first_obj = *std::get_if<Object*>(&first);
                if(second_obj && first_obj) {
                    // string with the same content should share the same StringObj in StringPool
                    pushStack(first_obj == second_obj);
                    break;
                }
                throw RuntimeError(getCurrentLine(), "== opr only support number, boolean or string");
            }
            case OpCode::OP_GREATER:
            case OpCode::OP_LESS: {
                second = popStack();
                first = popStack();
                auto second_double = std::get_if<double>(&second);
                auto first_double = std::get_if<double>(&first);
                if(second_double && first_double) {
                    pushStack(basic_boolean(currentOpcode, *first_double, *second_double));
                } else {
                    throw RuntimeError(getCurrentLine(), "<, > opr only support number");
                }
                break;
            }
            case OpCode::OP_POP: {
                popStack();
                break;
            }
            case OpCode::OP_CLOSE_UPVALUE: {
                closeUpValue();
                popStack();  // pop the closed local variable off stack!
                break;
            }
            case OpCode::OP_PRINT: {
                auto value = popStack();
                std::cout << toString(value) << std::endl;
                break;
            }
            case OpCode::OP_RETURN: {
                // returnValue should be on stackTop
                Value returnValue = popStack();
                // close all upValueObj with location >= &vm.stack[stackBase] before popping those values
//                closeUpValues(&vm.stack[stackBase]);
                while(vm.stack.size() > stackBase) { // pop all local variables for func call
                    closeUpValue();
                    vm.stack.pop_back();
                }
                vm.stack.push_back(returnValue);

                vm.callFrames.pop_back();
                return;
                break;
            }
            case OpCode::OP_DEFINE_GLOBAL: {
                StringObj* varName = std::get<Object*>(readConstant())->cast<StringObj>();
                vm.globals[varName] = peekStackTop(0);
                popStack();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                StringObj* varName = std::get<Object*>(readConstant())->cast<StringObj>();
                pushStack(vm.globals[varName]);
                break;
            }
            case OpCode::OP_GET_LOCAL: {
                uint8_t stackIndex = readByte();
                pushStack(peekStackBase(stackIndex));
                break;
            }
            case OpCode::OP_SET_GLOBAL: {
                StringObj* varName = std::get<Object*>(readConstant())->cast<StringObj>();
                auto iter = vm.globals.find(varName);
                if(iter != vm.globals.end()) {
                    iter->second = peekStackTop();
                } else {
                    throw RuntimeError(getCurrentLine(), "variable " + varName->toString() + " is not declared");
                }
                break;
            }
            case OpCode::OP_SET_LOCAL: {
                uint8_t index = readByte();
                setStackBase(index, peekStackTop());
                break;
            }
            case OpCode::OP_CALL: {
                uint8_t actualArity = readByte();
                auto value = peekStackTop(actualArity);
                opCallValue(value, actualArity);
                return;
                break;
            }
            case OpCode::OP_CLOSURE: {
                auto functionValue = readConstant();
                auto newFunction = castToObj<FunctionObj>(&functionValue);
                auto newClosure = GarbageCollector::getInstance().addObject(new ClosureObj(newFunction));
                addUpValuesToClosure(newClosure);
                pushStack(newClosure);
                break;
            }
            case OpCode::OP_GET_UPVALUE: {
                int upValueIndex = readByte();
                auto upValue = *closureObj->upValues[upValueIndex]->location;
                pushStack(upValue);
                break;
            }
            case OpCode::OP_CLASS: {
                Value classNameValue = readConstant();
                auto className = castToObj<StringObj>(&classNameValue);
                auto newClass = GarbageCollector::getInstance().addObject(new ClassObj(className));
                pushStack(newClass);
                break;
            }
            case OpCode::OP_SET_UPVALUE: {
                int upValueIndex = readByte();
                auto newValue = peekStackTop();
                *closureObj->upValues[upValueIndex]->location = newValue;
                break;
            }
            case OpCode::OP_GET_PROPERTY: {
                auto object = peekStackTop(0);
                auto instanceObj = castToObj<InstanceObj>(&object);
                if(instanceObj == nullptr) {
                    throw RuntimeError(getCurrentLine(), "only instance can get field");
                }
                auto fieldName = readConstant();
                auto field = castToObj<StringObj>(&fieldName);
                auto fieldIter = instanceObj->fields.find(field);
                if(fieldIter != instanceObj->fields.end()) {
                    popStack();
                    pushStack(fieldIter->second);
                    break;
                }
                auto methodIter = instanceObj->klass->methods.find(field);
                if(methodIter != instanceObj->klass->methods.end()) {
                    auto classMethod = GarbageCollector::getInstance().addObject(
                            new ClassMethodObj(instanceObj, methodIter->second));
                    popStack();
                    pushStack(classMethod);
                    break;
                }

                throw RuntimeError(getCurrentLine(),
                        toString(object) + " has no field " + toString(fieldName));
                break;
            }
            case OpCode::OP_SET_PROPERTY: {
                auto newValue = peekStackTop(1);
                auto object = peekStackTop(0);
                auto instanceObj = castToObj<InstanceObj>(&object);
                if(instanceObj == nullptr) {
                    std::cerr << "in : " << closureObj->toString() << std::endl;
                    throw RuntimeError(getCurrentLine(), "only instance can set field, but "
                                                         + toString(object) + " is not instance");
                }
                auto fieldName = readConstant();
                auto field = castToObj<StringObj>(&fieldName);
                instanceObj->fields[field] = newValue;
                popStack();
                break;
            }
            case OpCode::OP_METHOD: {
                auto nameValue = readConstant();
                auto methodName = castToObj<StringObj>(&nameValue);
                auto klassValue = peekStackTop(1);
                auto klass = castToObj<ClassObj>(&klassValue);
                auto methodValue = peekStackTop(0);
                auto method = castToObj<ClosureObj>(&methodValue);
                klass->methods[methodName] = method;
                popStack();
                break;
            }
            case OpCode::OP_INVOKE: {
                auto nameValue = readConstant();
                auto methodName = castToObj<StringObj>(&nameValue);
                auto actualArity = readByte();
                auto objectValue = peekStackTop(actualArity);
                auto object = castToObj<InstanceObj>(&objectValue);
                auto fieldIter = object->fields.find(methodName);
                if(fieldIter != object->fields.end()) {
                    auto fieldMethod = castToObj<ClosureObj>(&fieldIter->second);
                    if(fieldMethod == nullptr) {
                        throw RuntimeError(getCurrentLine(),
                                toString(fieldIter->second) + " of " + object->toString() +
                                " cannot be called as method");
                    }
                    opCallValue(fieldMethod, actualArity);
                    return;
                    break;
                }
                auto methodIter = object->klass->methods.find(methodName);
                if(methodIter != object->klass->methods.end()) {
                    opCallValue(methodIter->second, actualArity);
                    return;
                    break;
                }

                throw RuntimeError(getCurrentLine(), "cannot invoke ["
                    + toString(objectValue) + "]." + toString(nameValue));
                break;
            }
            case OpCode::OP_INHERIT: {
                auto superclassValue = peekStackTop(0);
                auto superclass = castToObj<ClassObj>(&superclassValue);
                auto klassValue = peekStackTop(1);
                auto klass = castToObj<ClassObj>(&klassValue);
                if(superclass == nullptr || klass == nullptr) {
                    throw RuntimeError(getCurrentLine(), toString(klassValue) +
                    " cannot inherit from " + toString(superclassValue));
                }
                for(const auto & pair: superclass->methods) {
                    klass->methods[pair.first] = pair.second;
                }
                popStack();
                break;
            }
            case OpCode::OP_GET_SUPER: {
                auto objectValue = peekStackBase(0);
                auto object = castToObj<InstanceObj>(&objectValue);
                if(object == nullptr) {
                    throw RuntimeError(getCurrentLine(),
                            "super can only be accessed within object method");
                }
                auto superclassValue = peekStackTop(0);
                auto superclass = castToObj<ClassObj>(&superclassValue);
                if(superclass == nullptr) {
                    throw RuntimeError(getCurrentLine(), "superclass not found");
                }
                auto nameValue = readConstant();
                auto methodName = castToObj<StringObj>(&nameValue);
                auto methodIter = superclass->methods.find(methodName);
                if(methodIter == superclass->methods.end()) {
                    std::string temp = " ----" + superclass->toString() + ": " + "\n";
                    for(auto & pair: superclass->methods) {
                        temp += pair.first->toString() + ", " + pair.second->toString() + "\n";
                    }
                    temp += " ---- \n";
                    throw RuntimeError(getCurrentLine(),
                            temp + "method " + toString(nameValue) + " not found in superclass " + toString(superclassValue));
                }
                auto method = methodIter->second;
                auto superMethod = GarbageCollector::getInstance().addObject(new ClassMethodObj(object, method));
                popStack();
                pushStack(superMethod);
                break;
            }
            case OpCode::OP_VMSTACK_DEBUG: {
                std::cerr << " @@@@ OP_VMSTACK_DEBUG" << std::endl;
                vm.stack.printStack();
                break;
            }
            default: {
                auto temp = static_cast<uint8_t >(currentOpcode);
                throw std::logic_error("unhandled Opcode in CallFrame: " + std::to_string(temp));
            }
        }
    }
}

void CallFrame::opCallValue(Value callee, int actualArity) {
    // ClosureObj ClassObj
    if(isObj<ClosureObj>(&callee)) {
        auto closure = castToObj<ClosureObj>(&callee);
        auto function = closure->functionObj;
        if(function->getArity() != actualArity) {
            std::string errMsg = "function " + function->getName() + " expects " +
                                 std::to_string(function->getArity()) + " argument(s), but got " +
                                 std::to_string(actualArity) + " argument(s)";
            throw RuntimeError(getCurrentLine(),
                               errMsg);
        }
        vm.createCallFrame(closure, actualArity);
    } else if(isObj<ClassObj>(&callee)) {
        auto classObj = castToObj<ClassObj>(&callee);
        auto instanceObj = GarbageCollector::getInstance().addObject(new InstanceObj(classObj));
        vm.stack.indexFromEnd(actualArity) = instanceObj;  // replacing original callee (classObj)

        // call init(...) if exists
        StringObj* init = StringPool::getInstance().getStringObj("init");
        auto initIter = classObj->methods.find(init);
        if(initIter != classObj->methods.end()) {
            vm.createCallFrame(initIter->second, actualArity);
        } else {
            if(actualArity != 0) {
                throw RuntimeError(getCurrentLine(), "default constructor has no parameter, "
                                                     "but you passed " + std::to_string(actualArity) + " arguments");
            }
        }

    } else if(isObj<ClassMethodObj>(&callee)) {
        auto classMethodObj = castToObj<ClassMethodObj>(&callee);
        vm.stack.indexFromEnd(actualArity) = classMethodObj->receiver; // GC can blacken receiver from vm.stack
        vm.createCallFrame(classMethodObj->method, actualArity);
    } else {
        throw std::logic_error("unhandled callee in opCallValue: " + toString(callee));
    }
}

void CallFrame::addUpValuesToClosure(ClosureObj* newClosure) {
    for(int i = 0; i < newClosure->closureCount; i++) {
        int upValueIndex = readByte();
        int isLocal = readByte();
        if(isLocal) {
            auto tmp = captureUpValue(upValueIndex);
            newClosure->upValues.push_back(tmp);
        } else {
            newClosure->upValues.push_back(closureObj->upValues[upValueIndex]);
        }
    }
}

UpValueObj* CallFrame::captureUpValue(int localVarIndex) {
    Value * ptr = &vm.stack[stackBase + localVarIndex];
    auto temp =  GarbageCollector::getInstance().addUpValue(ptr);
//    std::cerr << "captureUpValue(): " << toString(*ptr) << " by " << closureObj->toString()
//        << " with ptr " << temp << std::endl;
    return temp;
}

Chunk& CallFrame::getCurrentChunk() {
    return closureObj->functionObj->getChunk();
}

uint8_t CallFrame::readByte() {
    return getCurrentChunk().code[ip++];
}

OpCode CallFrame::readOpCode() {
    return static_cast<OpCode>(readByte());
}

Value CallFrame::readConstant() {
    auto constant_id = readByte();
    return getCurrentChunk().constants[constant_id];
}

int CallFrame::getCurrentLine() {
    return ip > 0 ? getCurrentChunk().lines[ip - 1] : 0;
}

Value CallFrame::popStack() {
    return vm.stack.pop_back();
}

void CallFrame::closeUpValue() {
    Value* ptr = &vm.stack.indexFromEnd(0);
    auto & upValues = GarbageCollector::getInstance().allOpenUpValues;
    while (upValues != nullptr && upValues->location >= ptr) {
        if(upValues->location == ptr) {
            upValues->closed = *ptr;
            upValues->location = &upValues->closed;
            break;
        }
        upValues = upValues->next;
    }
}

void CallFrame::closeUpValues(Value* location) {
    auto & upValues = GarbageCollector::getInstance().allOpenUpValues;
    while (upValues != nullptr && upValues->location >= location) {
        upValues->closed = *upValues->location;
        upValues->location = &upValues->closed;
    }
}

Value CallFrame::peekStackBase(int relativeIndex) {
    return vm.stack[stackBase + relativeIndex];
}

void CallFrame::pushStack(Value value) {
    vm.stack.push_back(value);
}

Value CallFrame::peekStackTop(int relativeIndex) {
#ifdef VECTOR
    return vm.stack.end()[-1-relativeIndex];
#else
    return vm.stack.indexFromEnd(relativeIndex);
#endif
}

void CallFrame::setStackBase(int relativeIndex, const Value &newValue) {
    vm.stack[stackBase + relativeIndex] = newValue;
}


