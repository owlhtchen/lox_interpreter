
#ifndef LOX_INTERPRETER_CODEGENERATOR_H
#define LOX_INTERPRETER_CODEGENERATOR_H

#include <VisitorExpr.h>
#include <Chunk.h>

class CodeGenerator: public VisitorExpr {
public:
    Chunk chunk;
private:
     void visitLiteralExpr(const LiteralExpr& expr) override;
     void visitUnaryExpr(const UnaryExpr& expr) override;
     void visitGroupingExpr(const GroupingExpr& expr) override;
     void visitBinaryExpr(const BinaryExpr& expr) override;
     void compile(Expr& expr);
};

#endif //LOX_INTERPRETER_CODEGENERATOR_H
