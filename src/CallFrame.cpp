
#include "CallFrame.h"
#include <Object.h>
#include <VM.h>
#include <util.h>
#include <Value.h>
#include <iostream>
#include <stdexcept>

void CallFrame::runFrame() {
    OpCode currentOpcode;
    Value second, first;
    auto& chunk = getCurrentChunk();

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
            case OpCode::OP_PRINT: {
                auto value = popStack();
                std::cout << toString(value) << std::endl;
                break;
            }
            case OpCode::OP_RETURN: {
                vm.callFrames.pop_back();
                return;
                break;
            }
            case OpCode::OP_DEFINE_GLOBAL: {
                StringObj* varName = std::get<Object*>(readConstant())->cast<StringObj>();
                vm.globals[varName] = popStack();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                StringObj* varName = std::get<Object*>(readConstant())->cast<StringObj>();
                pushStack(varName);
                break;
            }
            case OpCode::OP_GET_LOCAL: {
                uint8_t stackIndex = readByte();
                pushStack(peekStack(stackIndex));
                break;
            }
            default: {
                throw std::logic_error("unhandle Opcode in CallFrame");
            }
        }
    }
}

Chunk& CallFrame::getCurrentChunk() {
    return functionObj->getChunk();
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
    return getCurrentChunk().lines[ip - 1];
}

Value CallFrame::popStack() {
    auto temp = vm.stack.back();
    vm.stack.pop_back();
    return temp;
}

Value CallFrame::peekStack(int relativeIndex) {
    return vm.stack[stackBase + relativeIndex];
}

void CallFrame::pushStack(Value value) {
    vm.stack.push_back(value);
}


