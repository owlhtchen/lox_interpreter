
#ifndef LOX_INTERPRETER_FUNCTIONCOMPILER_H
#define LOX_INTERPRETER_FUNCTIONCOMPILER_H

#include <memory>
#include <vector>
#include <Local.h>
#include <cstdint>
#include <FunctionType.h>

class FunctionObj;

class FunctionCompiler {
public:
    explicit FunctionCompiler(std::shared_ptr<FunctionCompiler> enclosing, FunctionType funcType, std::string funcName = "");
    std::shared_ptr<FunctionCompiler> enclosing;
    FunctionObj* functionObj;
    //locals: this gives the index localVariables in CallFrame during runtime
    // (simulates the stack frame in runtime to some extent)
    std::vector<Local> locals;
    int currentScopeDepth;
    FunctionType functionType;
    void declareLocal(const Token& token);
    void addLocal(const Token& token);
    void markDefined();
    void beginScope();
    void endScope();
};


#endif //LOX_INTERPRETER_FUNCTIONCOMPILER_H
