
#include <VM.h>
#include <util.h>
#include <variant>
#include <Error.h>
#include <iostream>
#include <Object.h>

Chunk& VM::getCurrentChunk() {
    return chunk;
}

void VM::run() {
    OpCode currentOpcode;
    Value second, first;

    while(ip < chunk.code.size()) {
        currentOpcode = readOpCode();
        switch (currentOpcode) {
            case OpCode::OP_CONSTANT: {
                stack.push_back(readConstant());
                break;
            }
            case OpCode::OP_TRUE: {
                stack.emplace_back(true);
                break;
            }
            case OpCode::OP_FALSE: {
                stack.push_back(false);
                break;
            }
            case OpCode::OP_NIL: {
                stack.emplace_back(std::monostate());
                break;
            }
            case OpCode::OP_NEGATE: {
                first = popStack();
                if (auto first_double = std::get_if<double>(&first)) {
                    stack.push_back(-*first_double);
                } else {
                    throw RuntimeError(getCurrentLine(), "negate opr can only be used with a number");
                }
                break;
            }
            case OpCode::OP_NOT: {
                first = popStack();
                if (auto first_double = std::get_if<bool>(&first)) {
                    stack.push_back(!*first_double);
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
                    stack.push_back(basic_arithmetic(currentOpcode, *first_double, *second_double));
                } else if(currentOpcode == OpCode::OP_ADD) {
                    Object* second_obj = *std::get_if<Object*>(&second);
                    Object* first_obj = *std::get_if<Object*>(&first);
                    if(second_obj && first_obj) {
                        auto second_str = second_obj->dyn_cast<StringObj>();
                        auto first_str = first_obj->dyn_cast<StringObj>();
                        std::string new_str = first_str->str + second_str->str;
                        stack.push_back(StringPool::getInstance().getStringObj(new_str));
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
                    stack.push_back(basic_boolean(currentOpcode, *first_double, *second_double));
                    break;
                }
                auto second_bool = std::get_if<bool>(&second);
                auto first_bool = std::get_if<bool>(&first);
                if(second_bool && first_bool) {
                    stack.push_back(*first_bool == *second_bool);
                    break;
                }
                auto second_obj = *std::get_if<Object*>(&second);
                auto first_obj = *std::get_if<Object*>(&first);
                if(second_obj && first_obj) {
                    // string with the same content should share the same StringObj in StringPool
                    stack.push_back(first_obj == second_obj);
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
                    stack.push_back(basic_boolean(currentOpcode, *first_double, *second_double));
                } else {
                    throw RuntimeError(getCurrentLine(), "<, > opr only support number");
                }
                break;
            }
        }
    }
}

uint8_t VM::readByte() {
    return getCurrentChunk().code[ip++];
}

OpCode VM::readOpCode() {
    return static_cast<OpCode>(readByte());
}

Value VM::readConstant() {
    auto constant_id = readByte();
    return getCurrentChunk().constants[constant_id];
}

Value VM::peekStack(int relative_pos) {
    return *(stack.end() -1 - relative_pos);
}

int VM::getCurrentLine() {
    return chunk.lines[ip - 1];
}

Value VM::popStack() {
    Value temp = stack.back();
    stack.pop_back();
    return temp;
}
