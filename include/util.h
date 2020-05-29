
#ifndef LOX_INTERPRETER_UTIL_H
#define LOX_INTERPRETER_UTIL_H

#include <OpCode.h>
#include <stdexcept>

inline double basic_arithmetic(OpCode opCode, double first, double second) {
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

inline bool basic_boolean(OpCode opCode, double first, double second) {
    switch (opCode) {
        case OpCode::OP_EQUAL:
            return first - second < 1e-5 || second - first < 1e-5;
        case OpCode::OP_GREATER:
            return first > second;
        case OpCode::OP_LESS:
            return first < second;
        default:
            throw std::logic_error("unexpected opCode in basic_boolean");
    }
}


#endif //LOX_INTERPRETER_UTIL_H
