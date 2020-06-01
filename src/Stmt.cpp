
#include "Stmt.h"
#include <VisitorStmt.h>
#include <Expr.h>

void ExprStmt::accept(VisitorStmt &visitor) {
    visitor.visitExprStmt(*this);
}

void PrintStmt::accept(VisitorStmt &visitor) {
    visitor.visitPrintStmt(*this);
}

void VarDeclStmt::accept(VisitorStmt &visitor) {
    visitor.visitVarDeclStmt(*this);
}

void BlockStmt::accept(VisitorStmt &visitor) {
    visitor.visitBlockStmt(*this);
}
