
#ifndef LOX_INTERPRETER_STMTEXPR_H
#define LOX_INTERPRETER_STMTEXPR_H

class Stmt;
class ExprStmt;
class PrintStmt;
class VarDeclStmt;

class VisitorStmt {
public:
    virtual void visitExprStmt(const ExprStmt& expr) = 0;
    virtual void visitPrintStmt(const PrintStmt& expr) = 0;
    virtual void visitVarDeclStmt(const VarDeclStmt& expr) = 0;
};

#endif //LOX_INTERPRETER_STMTEXPR_H
