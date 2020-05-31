
#ifndef LOX_INTERPRETER_COMPILER_H
#define LOX_INTERPRETER_COMPILER_H

#include <memory>
#include <vector>
#include <Local.h>

class FunctionObj;

class Compiler {
public:
    std::shared_ptr<Compiler> enclosing;
    FunctionObj* functionObj;
    std::vector<Local> locals;  // localVariables
    int currentScopeDepth;
};


#endif //LOX_INTERPRETER_COMPILER_H
