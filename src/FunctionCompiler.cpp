
#include "FunctionCompiler.h"
#include <cstdint>
#include <utility>
#include <Error.h>
#include <DerivedObject.h>
#include <GarbageCollector.h>
#include <iostream>

void FunctionCompiler::declareLocal(const Token &token) {
    if(currentScopeDepth == 0) {
        return;
    }
    // check token.lexeme exists at currentScopeDepth or not, and add
    for(int i = locals.size(); i >= 0; i--) {
        if(locals[i].scopeDepth != -1 && locals[i].scopeDepth != currentScopeDepth) {
            // check done
            break;
        }
        if(locals[i].token.lexeme == token.lexeme) {
            throw RuntimeError(token.line, "local variable already declared at " +
            std::to_string(locals[i].token.line));
        }
    }
    addLocal(token);
}

void FunctionCompiler::addLocal(const Token &token) {
    // append token to locals
    if(locals.size() > UINT8_MAX) {
        throw CompileError(token, "too many local variables");
    }
    locals.emplace_back(token, -1);
}

void FunctionCompiler::markLocalDefined() {
    if(currentScopeDepth == 0) {
        return;
    }
    locals.back().scopeDepth = currentScopeDepth;
}

void FunctionCompiler::beginScope() {
    currentScopeDepth++;
}

void FunctionCompiler::endScope(int endLine) {
    currentScopeDepth--;
    auto& chunk = functionObj->getChunk();
    while (locals.size() >= 0 &&
    locals.back().scopeDepth > currentScopeDepth) {
        locals.pop_back();
        chunk.emitOpCode(OpCode::OP_POP, endLine);
    }
}

FunctionCompiler::FunctionCompiler(std::shared_ptr<FunctionCompiler> enclosing, FunctionType funcType,
        std::string funcName): enclosing(std::move(enclosing)), functionType(funcType) {
    currentScopeDepth = 0;
    functionObj = GarbageCollector::getInstance().addObject(new FunctionObj(std::move(funcName)));
    if(functionType == METHOD_TYPE) {
        locals.emplace_back(Token("this"), 0);
    } else {
        locals.emplace_back(Token(""), 0);
    }
}

int FunctionCompiler::resolveLocal(const Token& varName) {
//    for(auto i = locals.size() - 1; i >= 0; i--) {  DO NOT use auto here (auto is size_t, unsigned int, i-- doesn't work)
    for(int i = locals.size() - 1; i >= 0; i--) {
        if(varName.lexeme == locals[i].token.lexeme) {
            if (locals[i].scopeDepth < 0) {
                throw RuntimeError(varName.line, "variable " + varName.lexeme
                + " is declared but not defined");
            } else {
                return i;
            }
        }
    }
    return -1;
}

int FunctionCompiler::resolveUpValue(const Token &varName) {
    std::cout << "resolve upvalue " << varName.lexeme << "@ " << std::to_string(varName.line) << std::endl;
    int index = -1;
    if(enclosing != nullptr) {
        index = enclosing->resolveLocal(varName);
    }
    if(index != -1) { // varName is found in the immediate enclosing function => capture upValue at runtime
        std::cout << " !resolved immediate: " << varName.lexeme << " " << std::to_string(index) << std::endl;
        return addUpValue(index, true, varName.line);
    }

    if(enclosing != nullptr) {
        index = enclosing->resolveUpValue(varName);
    }
    if(index != -1) { // index of varName captured as upValue in enclosing.upValues
        std::cout << " !resolved recurse: " << varName.lexeme << " " << std::to_string(index) << std::endl;
        return addUpValue(index, false, varName.line);
    }
    return -1;
}

int FunctionCompiler::addUpValue(int upValueIndex, bool isLocal, int line) {
    // check whether this upValue already exists
    for(int i = 0; i < upValues.size(); i++) {
        const auto & upValue = upValues[i];
        if(upValue.index == upValueIndex && upValue.isLocal == isLocal) {
            return i;
        }
    }
    upValues.emplace_back(upValueIndex, isLocal);
    upValueIndex = upValues.size() - 1;
    if(upValueIndex > 255) {
        throw CompileError(line, "more than 255 UpValues (variables for closure) in one function");
    }
    std::cout << " added upvalue to " << functionObj->getName() << " :(index) "
    << std::to_string(upValueIndex) << std::endl;
    return upValueIndex;
}
