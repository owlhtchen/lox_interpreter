
#include "Compiler.h"
#include <cstdint>
#include <Error.h>
#include <Object.h>

void Compiler::declareLocal(const Token &token) {
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

void Compiler::addLocal(const Token &token) {
    // append token to locals
    if(locals.size() > UINT8_MAX) {
        throw CompileError(token, "too many local variables");
    }
    locals.emplace_back(token, -1);
}

void Compiler::markDefined() {
    locals.back().scopeDepth = currentScopeDepth;
}

void Compiler::beginScope() {
    currentScopeDepth++;
}

void Compiler::endScope() {
    currentScopeDepth--;
    while (locals.size() >= 0 &&
    locals.back().scopeDepth > currentScopeDepth) {
        locals.pop_back();
    }
}

