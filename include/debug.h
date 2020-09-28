
#ifndef LOX_INTERPRETER_DEBUG_H
#define LOX_INTERPRETER_DEBUG_H
#include <vector>
#include <string>
#include <OpCode.h>
#include <stdexcept>

extern std::vector<std::string> tokentype_to_string;
#ifndef VECTOR
// #define VECTOR
#endif

#ifndef GC_DEBUG_PRINT
// #define GC_DEBUG_PRINT
#endif

#ifndef CAll_GC_EVERY
// #define CAll_GC_EVERY
#endif

#endif //LOX_INTERPRETER_DEBUG_H
