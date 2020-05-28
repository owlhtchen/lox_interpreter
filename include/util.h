
#ifndef LOX_INTERPRETER_UTIL_H
#define LOX_INTERPRETER_UTIL_H

#include <OpCode.h>
#include <stdexcept>

double basic_arithmetic(OpCode opCode, double first, double second) {
    switch (opCode) {
        case OpCode::OP_ADD:
            return first + second;
        case OpCode::OP_SUBTRACT:
            return first - second;
        case OpCode::OP_MULTIPLY:
            return first * second;
        case OpCode::OP_DIVIDE:
            return first / second;
        default:
            throw std::logic_error("unexpected opCode in basic_arithmetic");
    }
}

#endif //LOX_INTERPRETER_UTIL_H
