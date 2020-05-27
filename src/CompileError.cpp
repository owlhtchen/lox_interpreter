
#include "CompileError.h"
#include <iostream>

const char *CompileError::what() const noexcept{
    std::string msg = "Compiler Error at [" + std::to_string(token.line) + "]: " + message;
    return msg.c_str();
}
