
#ifndef LOX_INTERPRETER_ERROR_H
#define LOX_INTERPRETER_ERROR_H

#include <exception>
#include <string>
#include <Token.h>

class CompileError:public std::exception {
    // error when generating bytecode from ast
private:
    std:: string error_msg;
public:
    CompileError(const Token& token, const std::string& message) {
        error_msg = "Compile Error at line [" + std::to_string(token.line) + "]: " + message;
    };
    CompileError(int line, const std::string& message) {
      error_msg = "Compile Error at line [" + std::to_string(line) + "]: " + message;
    };
    [[nodiscard]] const char* what() const noexcept override;
};

class SyntaxError:public std::exception {
    // error when generating ast
private:
    std::string message;
    Token token;
    std:: string error_msg;
public:
    SyntaxError(Token token, std::string message):
            token(std::move(token)), message(std::move(message)) {
        error_msg = "Syntax Error at [" + std::to_string(token.line) + "]: " + this->message;
    };
    [[nodiscard]] const char* what() const noexcept override;
};


class RuntimeError:public std::exception {
    // error when generating ast
private:
    std::string message;
    int line;
    std:: string error_msg;
public:
    RuntimeError(int line, std::string message):
            line(line), message(std::move(message)) {
        error_msg = "Runtime Error at line [" + std::to_string(line) + "]: " + this->message;
    };
    [[nodiscard]] const char* what() const noexcept override;
};

#endif //LOX_INTERPRETER_ERROR_H
