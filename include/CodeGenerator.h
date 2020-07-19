
#ifndef LOX_INTERPRETER_CODEGENERATOR_H
#define LOX_INTERPRETER_CODEGENERATOR_H

#include <VisitorExpr.h>
#include <VisitorStmt.h>
#include <Chunk.h>
#include <memory>
#include <unordered_map>
#include <Value.h>
#include <Token.h>
#include "DerivedObject.h"
#include <FunctionType.h>


class FunctionCompiler;
class FunctionObj;
class StringObj;
class GetExpr;
class SetExpr;
class FunctionStmt;

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
    void visitAssignExpr(const AssignExpr& expr) override ;
    void visitExprStmt(const ExprStmt& stmt) override;
    void visitPrintStmt(const PrintStmt& stmt) override;
    void visitVarDeclStmt(const VarDeclStmt& stmt) override;
    uint8_t declareVariable(const Token& token);
    void defineVariable(uint8_t varNameIndex, int line);
    void defineGlobal(uint8_t varIdentifierId, int line);
    void visitBlockStmt(const BlockStmt& stmt) override;
    void visitFunctionStmt(const FunctionStmt& functionStmt) override ;
    void visitCallExpr(const CallExpr& callExpr) override ;
    void visitReturnStmt(const ReturnStmt& returnStmt) override ;
    void visitClassStmt(const ClassStmt& classStmt) override ;
    void visitGetExpr(const GetExpr & getExpr) override ;
    void visitSetExpr(const SetExpr & setExpr) override ;
    void visitSuperExpr(const SuperExpr & superExpr) override ;
    FunctionObj* endCurrentCompiler(int line);
    void getVariable(const Token& token);  // emit OpCode to push variable onto stack
    void compileFunctionStmt(const FunctionStmt & functionStmt, FunctionType functionType);  // closureObj compiled from functionStmt will be on stack top
    int compileArguments(const std::vector<std::unique_ptr<Expr>> & arguments);
    void compileSuperclassField(const std::string& superclassFieldName, OpCode opCode, int line);
    void visitIfStmt(const IfStmt & stmt) override ;
    void visitWhileStmt(const WhileStmt& stmt) override ;
};

#endif //LOX_INTERPRETER_CODEGENERATOR_H
