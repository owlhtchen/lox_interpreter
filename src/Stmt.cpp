
#include "Stmt.h"
#include <VisitorStmt.h>
#include <Expr.h>

#include <utility>

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

void ClassStmt::accept(VisitorStmt &visitor) const {
    visitor.visitClassStmt(*this);
}

int ClassStmt::getLastLine() const {
    return lastLine;
}

void ClassStmt::setSuperclass(Token _superclass) {
    superclass = std::move(_superclass);
    hasSuperclass = true;
}

void IfStmt::accept(VisitorStmt & visitor) const {
    visitor.visitIfStmt(*this);
}

int IfStmt::getLastLine() const {
    if(elseStmt) {
        return elseStmt->getLastLine();
    } else {
        return thenStmt->getLastLine();
    }
}

void WhileStmt::accept(VisitorStmt &visitor) const {
    visitor.visitWhileStmt(*this);
}

int WhileStmt::getLastLine() const {
    return body->getLastLine();
}
