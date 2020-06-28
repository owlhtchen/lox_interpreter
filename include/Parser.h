
#ifndef LOX_INTERPRETER_PARSER_H
#define LOX_INTERPRETER_PARSER_H

#include <vector>
#include <string>
#include <Token.h>
#include <memory>
#include <initializer_list>
#include <Expr.h>
#include <Stmt.h>

class Parser {
private:
    int current;
    std::vector<Token> tokens;
    bool hasError;
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    explicit Parser(std::vector<Token> tokens):
        current(0), tokens(std::move(tokens)), hasError(false) {};
    void parse();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> addition();
    std::unique_ptr<Expr> multiplication();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> block(int begin);
    bool isAtEnd();
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types); // e.g. checkType({TOKEN_MINUS, TOKEN_BANG})
    void consume(TokenType type, std::string err_msg); // the Error thrown from here should be caught in parseStmt to synchronize
    Token peek(int relative_pos);
};


#endif //LOX_INTERPRETER_PARSER_H
