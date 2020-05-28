
#include "Error.h"
#include <iostream>

const char *CompileError::what() const noexcept{
    std::string msg = "Compiler Error at [" + std::to_string(token.line) + "]: " + message;
    return msg.c_str();
}

const char *SyntaxError::what() const noexcept{
    std::string msg = "Syntax Error at [" + std::to_string(token.line) + "]: " + message;
    return msg.c_str();
}

const char *RuntimeError::what() const noexcept{
    std::string msg = "Runtime Error at [" + std::to_string(line) + "]: " + message;
    return msg.c_str();
}
