
#ifndef LOX_INTERPRETER_LOCAL_H
#define LOX_INTERPRETER_LOCAL_H

#include <Token.h>
class FunctionCompiler;

class Local {
    friend class FunctionCompiler;
private:
    Token token;
    int scopeDepth;
public:
    Local(Token token, int scopeDepth):
        token(std::move(token)), scopeDepth(scopeDepth) { };
};


#endif //LOX_INTERPRETER_LOCAL_H
