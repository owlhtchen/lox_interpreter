
#include "FunctionCompiler.h"
#include <cstdint>
#include <utility>
#include <Error.h>
#include <Object.h>
#include <GarbageCollector.h>

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

void FunctionCompiler::markDefined() {
    locals.back().scopeDepth = currentScopeDepth;
}

void FunctionCompiler::beginScope() {
    currentScopeDepth++;
}

void FunctionCompiler::endScope() {
    currentScopeDepth--;
    while (locals.size() >= 0 &&
    locals.back().scopeDepth > currentScopeDepth) {
        locals.pop_back();
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

