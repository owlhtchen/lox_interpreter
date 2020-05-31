
#ifndef LOX_INTERPRETER_CODEGENERATOR_H
#define LOX_INTERPRETER_CODEGENERATOR_H

#include <VisitorExpr.h>
#include <VisitorStmt.h>
#include <Chunk.h>
#include <memory>

class Compiler;
class FunctionObj;

class CodeGenerator: public VisitorExpr, public VisitorStmt {
public:
//    Chunk chunk;
    std::shared_ptr<Compiler> currentCompiler;
    void compile(Expr& expr);
    FunctionObj* topFunction;
private:
    Chunk* getCurrentChunk();
    void visitLiteralExpr(const LiteralExpr& expr) override;
     void visitUnaryExpr(const UnaryExpr& expr) override;
     void visitGroupingExpr(const GroupingExpr& expr) override;
     void visitBinaryExpr(const BinaryExpr& expr) override;
     void visitExprStmt(const ExprStmt& expr) override;
     void visitPrintStmt(const PrintStmt& expr) override;
     void visitVarDeclStmt(const VarDeclStmt& expr) override;
};

#endif //LOX_INTERPRETER_CODEGENERATOR_H
