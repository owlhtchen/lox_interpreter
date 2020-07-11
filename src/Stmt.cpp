
#include "Stmt.h"
#include <VisitorStmt.h>
#include <Expr.h>

void ExprStmt::accept(VisitorStmt &visitor) const {
    visitor.visitExprStmt(*this);
}

int ExprStmt::getLastLine() const {
    return line;
}

void PrintStmt::accept(VisitorStmt &visitor) const {
    visitor.visitPrintStmt(*this);
}

int PrintStmt::getLastLine() const {
    return line;
}

void VarDeclStmt::accept(VisitorStmt &visitor) const {
    visitor.visitVarDeclStmt(*this);
}

int VarDeclStmt::getLastLine() const {
    return varToken.line;
}

void BlockStmt::accept(VisitorStmt &visitor) const {
    visitor.visitBlockStmt(*this);
}

int BlockStmt::getLastLine() const {
    return end;
}

void FunctionStmt::accept(VisitorStmt &visitor) const {
    visitor.visitFunctionStmt(*this);
}

int FunctionStmt::getLastLine() const {
    return body.empty() ? 0 : body.back()->getLastLine();
}

void ReturnStmt::accept(VisitorStmt &visitor) const {
    visitor.visitReturnStmt(*this);
}

int ReturnStmt::getLastLine() const {
    return returnKeyword.line;
}
