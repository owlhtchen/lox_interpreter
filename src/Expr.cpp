
#include <cstdlib>
#include <iostream>
#include <Expr.h>
#include <VisitorExpr.h>
#include <Token.h>

LiteralExpr::LiteralExpr(Token token) {
    switch (token.type) {
        case TOKEN_NUMBER:
            value = atof(token.lexeme.c_str());
            break;
    }

    std::cerr << "Should not reach here [Expr.cpp]" << std::endl;
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