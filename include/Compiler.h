
#ifndef LOX_INTERPRETER_COMPILER_H
#define LOX_INTERPRETER_COMPILER_H

#include <memory>
#include <vector>
#include <Local.h>
#include <cstdint>

class FunctionObj;

class Compiler {
public:
    std::shared_ptr<Compiler> enclosing;
    FunctionObj* functionObj;
    //locals: this gives the index localVariables in CallFrame during runtime
    // (simulates the stack frame in runtime to some extent)
    std::vector<Local> locals;
    int currentScopeDepth;
    void declareLocal(const Token& token);
    void addLocal(const Token& token);
    void markDefined();
    void beginScope();
    void endScope();
};


#endif //LOX_INTERPRETER_COMPILER_H
