
#ifndef LOX_INTERPRETER_CODEGENERATOR_H
#define LOX_INTERPRETER_CODEGENERATOR_H

#include <VisitorExpr.h>
#include <VisitorStmt.h>
#include <Chunk.h>
#include <memory>
#include <unordered_map>
#include <Value.h>
#include <Token.h>
#include "Object.h"


class FunctionCompiler;
class FunctionObj;
class StringObj;

class CodeGenerator: public VisitorExpr, public VisitorStmt {
public:
    // Chunk chunk;
    // one compiler for one function
    // FunctionObj* topFunction;
    std::shared_ptr<FunctionCompiler> currentCompiler;
    void compileExpr(const Expr& expr);
    void compileStmt(const Stmt& stmt);
    FunctionObj* compile(const std::vector<std::unique_ptr<Stmt>>& statements);
private:
    Chunk* getCurrentChunk();
    void visitLiteralExpr(const LiteralExpr& expr) override;
    void visitUnaryExpr(const UnaryExpr& expr) override;
    void visitGroupingExpr(const GroupingExpr& expr) override;
    void visitBinaryExpr(const BinaryExpr& expr) override;
    void visitExprStmt(const ExprStmt& stmt) override;
    void visitPrintStmt(const PrintStmt& stmt) override;
    void visitVarDeclStmt(const VarDeclStmt& stmt) override;
    uint8_t parseVariable(const Token& token);
    void defineGlobal(uint8_t varIdentifierId, int line);
    void visitBlockStmt(const BlockStmt& stmt) override;
};

#endif //LOX_INTERPRETER_CODEGENERATOR_H
