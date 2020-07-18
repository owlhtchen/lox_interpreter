
#ifndef LOX_INTERPRETER_LOCAL_H
#define LOX_INTERPRETER_LOCAL_H

#include <Token.h>
class FunctionCompiler;

class Local {
    friend class FunctionCompiler;
private:
    Token token;
    int scopeDepth;
    bool isCaptured;
public:
    Local(Token token, int scopeDepth):
        token(std::move(token)), scopeDepth(scopeDepth), isCaptured(false) { };
    Token getToken();
};


#endif //LOX_INTERPRETER_LOCAL_H
