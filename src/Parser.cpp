
#include <Parser.h>
#include <Expr.h>
#include <Error.h>
#include <stdexcept>
#include <utility>

void Parser::parse() {
    while (!isAtEnd()) {
        statements.push_back(std::move(declaration()));
    }
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> left = comparison();
    while (match({TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL})) {
        Token opr = peek(-1);
        std::unique_ptr<Expr> right = comparison();
        left = std::make_unique<BinaryExpr>(opr, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> left = addition();
    while (match({TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL})) {
        Token opr = peek(-1);
        std::unique_ptr<Expr> right = addition();
        left = std::make_unique<BinaryExpr>(opr, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::addition() {
    std::unique_ptr<Expr> left = multiplication();
    while (match({TOKEN_MINUS, TOKEN_PLUS})) {
        Token opr = peek(-1);
        std::unique_ptr<Expr> right = multiplication();
        left = std::make_unique<BinaryExpr>(opr, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::multiplication() {
    std::unique_ptr<Expr> left = unary();
    while (match({TOKEN_SLASH, TOKEN_STAR})) {
        Token opr = peek(-1);
        std::unique_ptr<Expr> right = unary();
        left = std::make_unique<BinaryExpr>(opr, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::unary() {
    if(match({TOKEN_BANG, TOKEN_MINUS})) {
        Token opr = peek(-1);
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(opr, std::move(right));
    } else {
        return primary();
    }
}

std::unique_ptr<Expr> Parser::primary() {
    if(match({TOKEN_NUMBER, TOKEN_STRING, TOKEN_TRUE, TOKEN_FALSE, TOKEN_NIL})) {
        Token literal = peek(-1);
        return std::make_unique<LiteralExpr>(literal);
    } else if(match(TOKEN_LEFT_PAREN)) {
        auto innerExpr = expression();
        consume(TOKEN_RIGHT_PAREN, "Missing )");
        return std::make_unique<GroupingExpr>(std::move(innerExpr));
    }

    throw std::logic_error("primary nullptr in parser: with token "
          + tokens[current].lexeme + " of type: " + std::to_string(tokens[current].type));
}

bool Parser::isAtEnd() {
    return current >= tokens.size();
}

bool Parser::match(TokenType type) {
    if(isAtEnd()) return false;
    if(tokens[current].type != type) {
        return false;
    }
    current++;
    return true;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    if(isAtEnd()) return false;
    for(const auto& type: types) {
        if(type == tokens[current].type) {
            current++;
            return true;
        }
    }
    return false;
}

Token Parser::peek(int relative_pos)  {
    return tokens[current + relative_pos];
}

void Parser::consume(TokenType type, std::string err_msg ) {
    if(!match(type)) {
        hasError = true;
        throw SyntaxError(tokens[current], std::move(err_msg));
    }
}

std::unique_ptr<Stmt> Parser::declaration() {
    if(match(TOKEN_VAR)) { // varDecl
        Token varToken = tokens[current++];
        std::unique_ptr<Expr> expr = nullptr;
        if(match(TOKEN_EQUAL)) {
            expr = expression();
        }
        return std::make_unique<VarDeclStmt>(varToken, std::move(expr));
    } else { // statement
        return statement();
    }
}

std::unique_ptr<Stmt> Parser::statement() {
    if(match(TOKEN_PRINT)) { // printStmt;
        auto expr = expression();
        consume(TOKEN_SEMICOLON, "expected ;");
        return std::make_unique<PrintStmt>(std::move(expr), tokens[current - 1].line);
    } else { // exprStmt;
        auto expr = expression();
        consume(TOKEN_SEMICOLON, "expected ;");
        return std::make_unique<ExprStmt>(std::move(expr), tokens[current - 1].line);
    }
}


