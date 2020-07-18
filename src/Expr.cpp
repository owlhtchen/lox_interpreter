
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
    && token.type != TOKEN_IDENTIFIER
    && token.type != TOKEN_THIS
    ) {
        throw SyntaxError(token, "unexpected token " + this->token.lexeme + " as LiteralExpr");
    }
}

void LiteralExpr::accept(VisitorExpr& visitor) const {
    visitor.visitLiteralExpr(*this);
}

int LiteralExpr::getLastLine() const {
    return token.line;
}

void UnaryExpr::accept(VisitorExpr& visitor) const {
    visitor.visitUnaryExpr(*this);
}

int UnaryExpr::getLastLine() const {
    return right->getLastLine();
}

void GroupingExpr::accept(VisitorExpr& visitor) const {
    visitor.visitGroupingExpr(*this);
}

int GroupingExpr::getLastLine() const {
    return innerExpr->getLastLine();
}

void BinaryExpr::accept(VisitorExpr& visitor) const {
    visitor.visitBinaryExpr(*this);
}

int BinaryExpr::getLastLine() const {
    return right->getLastLine();
}

void AssignExpr::accept(VisitorExpr &visitor) const {
    visitor.visitAssignExpr(*this);
}

int AssignExpr::getLastLine() const {
    return variable.line;
}

void CallExpr::accept(VisitorExpr &visitor) const {
    visitor.visitCallExpr(*this);
}

int CallExpr::getLastLine() const {
    return line;
}

void GetExpr::accept(VisitorExpr &visitor) const {
    visitor.visitGetExpr(*this);
}

int GetExpr::getLastLine() const {
    return field.line;
}

void SetExpr::accept(VisitorExpr &visitor) const {
    visitor.visitSetExpr(*this);
}

int SetExpr::getLastLine() const {
    return field.line;
}

int SuperExpr::getLastLine() const {
    return line;
}

void SuperExpr::accept(VisitorExpr &visitor) const {
    visitor.visitSuperExpr(*this);
}
