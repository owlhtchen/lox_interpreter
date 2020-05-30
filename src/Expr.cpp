
#include <cstdlib>
#include <iostream>
#include <Expr.h>
#include <VisitorExpr.h>
#include <Error.h>

LiteralExpr::LiteralExpr(const Token& token):token(token) {
    if(token.type != TOKEN_NUMBER
    && token.type != TOKEN_NIL
    && token.type != TOKEN_TRUE
    && token.type != TOKEN_FALSE
    && token.type != TOKEN_STRING
    ) {
        throw SyntaxError(token, "unexpected token " + this->token.lexeme + " as LiteralExpr");
    }
}

void LiteralExpr::accept(VisitorExpr& visitor) {
    visitor.visitLiteralExpr(*this);
}

void UnaryExpr::accept(VisitorExpr& visitor) {
    visitor.visitUnaryExpr(*this);
}

void GroupingExpr::accept(VisitorExpr& visitor) {
    visitor.visitGroupingExpr(*this);
}

void BinaryExpr::accept(VisitorExpr& visitor) {
    visitor.visitBinaryExpr(*this);
}