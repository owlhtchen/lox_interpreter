
#include "Stmt.h"
#include <VisitorStmt.h>
#include <Expr.h>

void ExprStmt::accept(VisitorStmt &visitor) const {
    visitor.visitExprStmt(*this);
}

void PrintStmt::accept(VisitorStmt &visitor) const {
    visitor.visitPrintStmt(*this);
}

void VarDeclStmt::accept(VisitorStmt &visitor) const {
    visitor.visitVarDeclStmt(*this);
}

void BlockStmt::accept(VisitorStmt &visitor) const {
    visitor.visitBlockStmt(*this);
}
