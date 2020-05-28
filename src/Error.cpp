
#include "Error.h"
#include <iostream>

const char *CompileError::what() const noexcept{
//    std::string error_msg = "Compiler Error at [" + std::to_string(token.line) + "]: " + message;
    return error_msg.c_str();
}

const char *SyntaxError::what() const noexcept{
//    std::string error_msg = "Syntax Error at [" + std::to_string(token.line) + "]: " + message;
    return error_msg.c_str();
}

const char *RuntimeError::what() const noexcept{
//    std::string error_msg = "Runtime Error at [" + std::to_string(line) + "]: " + message;
    return error_msg.c_str();
}
