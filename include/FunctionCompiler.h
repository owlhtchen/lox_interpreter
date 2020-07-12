
#ifndef LOX_INTERPRETER_FUNCTIONCOMPILER_H
#define LOX_INTERPRETER_FUNCTIONCOMPILER_H

#include <memory>
#include <vector>
#include <Local.h>
#include <cstdint>
#include <FunctionType.h>
#include <UpValue.h>

class FunctionObj;
class Token;

class FunctionCompiler {
public:
    explicit FunctionCompiler(std::shared_ptr<FunctionCompiler> enclosing,
            FunctionType funcType, std::string funcName = "");
    std::shared_ptr<FunctionCompiler> enclosing;
    FunctionObj* functionObj;
    //locals: this gives the index localVariables in CallFrame during runtime
    // (simulates the stack frame in runtime to some extent)
    std::vector<Local> locals;
    int currentScopeDepth;
    FunctionType functionType;
    std::vector<UpValue> upValues;
    void declareLocal(const Token& token);
    void addLocal(const Token& token);
    void markLocalDefined();
    void beginScope();
    void endScope(int endLine);
    int resolveLocal(const Token& varName);
    int resolveUpValue(const Token& varName);
    int addUpValue(int upValueIndex, bool isLocal, int line);
};


#endif //LOX_INTERPRETER_FUNCTIONCOMPILER_H
