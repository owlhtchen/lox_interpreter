
#include <cstdlib>
#include <iostream>
#include <Expr.h>
#include <VisitorExpr.h>
#include <Error.h>

LiteralExpr::LiteralExpr(const Token& token):token(token) {
    if(token.type != TOKEN_NUMBER) {
        throw SyntaxError(token, "unexpected token as LiteralExpr");
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