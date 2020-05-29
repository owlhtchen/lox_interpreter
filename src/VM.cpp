
#include <VM.h>
#include <util.h>
#include <variant>
#include <Error.h>
#include <iostream>

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
                    // TODO:add string
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
                // TODO: add support for string
                throw RuntimeError(getCurrentLine(), "== opr only support number");
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
