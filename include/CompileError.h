
#ifndef LOX_INTERPRETER_COMPILEERROR_H
#define LOX_INTERPRETER_COMPILEERROR_H

#include <exception>
#include <string>
#include <Token.h>

class CompileError:public std::exception {
private:
    std::string message;
    Token token;
public:
    CompileError(Token token, std::string message):
        token(std::move(token)), message(std::move(message)) {};
    [[nodiscard]] const char* what() const noexcept override;
};


#endif //LOX_INTERPRETER_COMPILEERROR_H
