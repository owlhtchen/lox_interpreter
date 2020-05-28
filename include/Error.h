
#ifndef LOX_INTERPRETER_ERROR_H
#define LOX_INTERPRETER_ERROR_H

#include <exception>
#include <string>
#include <Token.h>

class CompileError:public std::exception {
    // error when generating bytecode from ast
private:
    std::string message;
    Token token;
public:
    CompileError(Token token, std::string message):
        token(std::move(token)), message(std::move(message)) {};
    [[nodiscard]] const char* what() const noexcept override;
};

class SyntaxError:public std::exception {
    // error when generating ast
private:
    std::string message;
    Token token;
public:
    SyntaxError(Token token, std::string message):
            token(std::move(token)), message(std::move(message)) {};
    [[nodiscard]] const char* what() const noexcept override;
};


class RuntimeError:public std::exception {
    // error when generating ast
private:
    std::string message;
    int line;
public:
    RuntimeError(int line, std::string message):
            line(line), message(std::move(message)) {};
    [[nodiscard]] const char* what() const noexcept override;
};

#endif //LOX_INTERPRETER_ERROR_H
