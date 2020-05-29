
#ifndef LOX_INTERPRETER_EXPR_H
#define LOX_INTERPRETER_EXPR_H

#include <memory>
#include <Token.h>
#include <Value.h>
class VisitorExpr;

class Expr {
public:
    virtual void accept(VisitorExpr& visitor) = 0;
    virtual ~Expr() = default;
};

// (-100 + 10) * 20 - 9 = -1809
// LiteralExpr, UnaryExpr, GroupingExpr, BinaryExpr
class LiteralExpr: public Expr {
public:
//    Value value;
    Token token;
    explicit LiteralExpr(const Token& token);
    void accept(VisitorExpr& visitor) override ;
    ~LiteralExpr() override = default;
};

class UnaryExpr: public Expr {
public:
    Token opr;  // opr for operator
    std::unique_ptr<Expr> right;
    UnaryExpr(Token opr, std::unique_ptr<Expr> right):
        opr(std::move(opr)), right(std::move(right)) {};
    void accept(VisitorExpr& visitor) override ;
    ~UnaryExpr() override = default;
};

class GroupingExpr: public Expr {
public:
    std::unique_ptr<Expr> innerExpr;
    explicit GroupingExpr(std::unique_ptr<Expr> innerExpr): innerExpr(std::move(innerExpr)) {};
    void accept(VisitorExpr& visitor) override ;
    ~GroupingExpr() override = default;
};

class BinaryExpr: public Expr {
public:
    Token opr;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    explicit BinaryExpr(Token opr, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right):
        opr(std::move(opr)), left(std::move(left)), right(std::move(right)) {};
    void accept(VisitorExpr& visitor) override ;
    ~BinaryExpr() override = default;
};

#endif //LOX_INTERPRETER_EXPR_H
