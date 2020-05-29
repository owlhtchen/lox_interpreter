
#include "Error.h"
#include <iostream>

const char *CompileError::what() const noexcept{
    return error_msg.c_str();
}

const char *SyntaxError::what() const noexcept{
    return error_msg.c_str();
}

const char *RuntimeError::what() const noexcept{
    return error_msg.c_str();
}
