
#ifndef LOX_INTERPRETER_STMTEXPR_H
#define LOX_INTERPRETER_STMTEXPR_H

class Stmt;
class ExprStmt;
class PrintStmt;
class VarDeclStmt;
class BlockStmt;
class FunctionStmt;
class ReturnStmt;
class ClassStmt;
class IfStmt;
class WhileStmt;
class ForStmt;

class VisitorStmt {
public:
    virtual void visitExprStmt(const ExprStmt& expr) = 0;
    virtual void visitPrintStmt(const PrintStmt& expr) = 0;
    virtual void visitVarDeclStmt(const VarDeclStmt& expr) = 0;
    virtual void visitBlockStmt(const BlockStmt& expr) = 0;
    virtual void visitFunctionStmt(const FunctionStmt& stmt) = 0;
    virtual void visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual void visitClassStmt(const ClassStmt& stmt) = 0;
    virtual void visitIfStmt(const IfStmt & stmt) = 0;
    virtual void visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual void visitForStmt(const ForStmt& stmt) = 0;
};

#endif //LOX_INTERPRETER_STMTEXPR_H
