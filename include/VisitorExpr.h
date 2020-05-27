
#ifndef LOX_INTERPRETER_VISITOREXPR_H
#define LOX_INTERPRETER_VISITOREXPR_H

class  Expr;
class LiteralExpr;
class UnaryExpr;
class GroupingExpr;
class BinaryExpr;

class VisitorExpr {
public:
    virtual void visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual void visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual void visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual void visitBinaryExpr(const BinaryExpr& expr) = 0;
};

#endif //LOX_INTERPRETER_VISITOREXPR_H
