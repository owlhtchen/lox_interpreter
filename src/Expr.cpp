
#include <cstdlib>
#include <iostream>
#include <Expr.h>
#include <VisitorExpr.h>
#include <cassert>

LiteralExpr::LiteralExpr(const Token& token):token(token) {
    assert(token.type == TOKEN_NUMBER);
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