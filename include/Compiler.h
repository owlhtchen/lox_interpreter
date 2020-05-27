
#ifndef LOX_INTERPRETER_COMPILER_H
#define LOX_INTERPRETER_COMPILER_H

#include <VisitorExpr.h>
#include <Chunk.h>

class Compiler: public VisitorExpr {
public:
    Chunk chunk;
private:
     void visitLiteralExpr(const LiteralExpr& expr) override;
     void visitUnaryExpr(const UnaryExpr& expr) override;
     void visitGroupingExpr(const GroupingExpr& expr) override;
     void visitBinaryExpr(const BinaryExpr& expr) override;
};

#endif //LOX_INTERPRETER_COMPILER_H
