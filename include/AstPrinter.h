
#ifndef LOX_INTERPRETER_ASTPRINTER_H
#define LOX_INTERPRETER_ASTPRINTER_H

#include <VisitorExpr.h>
#include <initializer_list>
#include <string>

class AstPrinter: public VisitorExpr {  // incomplete, only for debug
public:
    void printAst(Expr& expr);
private:
    void visitLiteralExpr(const LiteralExpr& expr) override;
    void visitUnaryExpr(const UnaryExpr& expr) override;
    void visitGroupingExpr(const GroupingExpr& expr) override;
    void visitBinaryExpr(const BinaryExpr& expr) override;
};


#endif //LOX_INTERPRETER_ASTPRINTER_H
