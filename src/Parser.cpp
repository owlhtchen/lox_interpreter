
#include <Parser.h>
#include <Expr.h>
#include <Error.h>
#include <stdexcept>
#include <utility>
#include <iostream>

void Parser::parse() {
    while (!isAtEnd()) {
        statements.push_back(std::move(declaration()));
    }
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = equality();
    if(match(TOKEN_EQUAL)) {
        auto* tmp = dynamic_cast<LiteralExpr*>(expr.get());
        if(tmp != nullptr) {
            return std::make_unique<AssignExpr>(tmp->token, expression());
        } else {
            throw RuntimeError(expr->getLastLine(), "invalid assignment target");
        }
    } else {
        return expr;
    }
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
    // unary → ( "!" | "-" ) unary | call ;
    if(match({TOKEN_BANG, TOKEN_MINUS})) {
        Token opr = peek(-1);
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(opr, std::move(right));
    } else {
        return call();
    }
}

std::unique_ptr<Expr> Parser::call() {
    // call  → primary ( "(" arguments? ")" )* ; // including f("a")("b")("c")
    std::unique_ptr<Expr> expr = primary();
    int line;
    std::vector<std::unique_ptr<Expr>> arguments;
    while(match(TOKEN_LEFT_PAREN)) {
        line = peek(-1).line;
        if(peek(0).type != TOKEN_RIGHT_PAREN) {
            do {
                arguments.emplace_back(expression());
            } while(match(TOKEN_COMMA));
        }
        consume(TOKEN_RIGHT_PAREN, ") expected for function call, but get " + peek(-1).lexeme);
        expr = std::make_unique<CallExpr>(std::move(expr), std::move(arguments), line);
    }
    return expr;
}


std::unique_ptr<Expr> Parser::primary() {
    if(match({TOKEN_NUMBER, TOKEN_STRING, TOKEN_TRUE, TOKEN_FALSE, TOKEN_NIL, TOKEN_IDENTIFIER})) {
        Token literal = peek(-1);
        return std::make_unique<LiteralExpr>(literal);
    } else if(match(TOKEN_LEFT_PAREN)) {
        auto innerExpr = expression();
        consume(TOKEN_RIGHT_PAREN, "Missing )");
        return std::make_unique<GroupingExpr>(std::move(innerExpr));
    }

    throw std::logic_error("primary nullptr in parser: with token "
          + tokens[current].lexeme + " of type: " + std::to_string(tokens[current].type) + " on line "
          + std::to_string(tokens[current].line));
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

std::unique_ptr<Stmt> Parser::funcDecl() {
    Token funcName = tokens[current++];
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
    consume(TOKEN_LEFT_PAREN, "( expected for function declaration");
    if(!isAtEnd() && peek(0).type != TOKEN_RIGHT_PAREN) {
        do {
            if(match(TOKEN_IDENTIFIER)) {
                params.push_back(peek(-1));
                if(peek(0).type != TOKEN_COMMA) {
                    break;
                }
            } else {
                throw SyntaxError(peek(0),
                                  "expect identifier token for function parameter, but get " + peek(0).lexeme);
            }
        } while(match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, ") expected for function declaration");
    consume(TOKEN_LEFT_BRACE, "expect { for function body");
    while(peek(0).type != TOKEN_RIGHT_BRACE) {
        body.push_back(declaration());
    }
    consume(TOKEN_RIGHT_BRACE, "expect } for function body");
    return std::make_unique<FunctionStmt>(funcName, std::move(params), std::move(body));
}

std::unique_ptr<Stmt> Parser::declaration() {
    if(match(TOKEN_VAR)) { // varDecl
        Token varToken = tokens[current++];
        std::unique_ptr<Expr> expr = nullptr;
        if(match(TOKEN_EQUAL)) {
            expr = expression();
        }
        current++; // consume ; at the end
        return std::make_unique<VarDeclStmt>(varToken, std::move(expr));
    } else if(match(TOKEN_FUN)) { // function declaration
        return funcDecl();
    } else { // statement
        return statement();
    }
}

std::unique_ptr<Stmt> Parser::statement() {
    if(match(TOKEN_PRINT)) { // printStmt;
        auto expr = expression();
        consume(TOKEN_SEMICOLON, "expected ;");
        return std::make_unique<PrintStmt>(std::move(expr), tokens[current - 1].line);
    } else if (match(TOKEN_LEFT_BRACE)) {
        return block(peek(-1).line);
    } else if(match(TOKEN_RETURN)) {
        std::unique_ptr<Expr> returnExpr = nullptr;
        if(peek(0).type != TOKEN_SEMICOLON) {
            returnExpr = expression();
        }
        consume(TOKEN_SEMICOLON, "; expected after return statement");
        return std::make_unique<ReturnStmt>(peek(-1), std::move(returnExpr));
    } else { // exprStmt;
        auto expr = expression();
        consume(TOKEN_SEMICOLON, "expected ;");
        return std::make_unique<ExprStmt>(std::move(expr), tokens[current - 1].line);
    }
}

std::unique_ptr<Stmt> Parser::block(int begin) {
    std::unique_ptr<BlockStmt> blockStmt(new BlockStmt);
    blockStmt->begin = begin;
    while(!isAtEnd() && peek(0).type != TOKEN_RIGHT_BRACE) {
        blockStmt->statements.push_back(declaration());
    }
    consume(TOKEN_RIGHT_BRACE, " unclosed block, expected }");
    blockStmt->end = peek(-1).line;
    return blockStmt;
}


