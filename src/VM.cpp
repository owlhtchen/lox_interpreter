
#include <VM.h>
#include <util.h>
#include <variant>
#include <Error.h>

Chunk& VM::getCurrentChunk() {
    return chunk;
}

void VM::run() {
    OpCode currentOpcode;
    Value second, first;

    while(ip < chunk.code.size()) {
        currentOpcode = readOpCode();
        switch (currentOpcode) {
            case OpCode::OP_CONSTANT:
                stack.push_back(readConstant());
                break;
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
                    throw RuntimeError(getCurrentLine(), "binary opr only support number or string (for add)");
                }
                break;
            }
            case OpCode::OP_NEGATE: {
                first = popStack();
                if(auto first_double = std::get_if<double>(&first)) {
                    stack.push_back(- *first_double);
                }
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
