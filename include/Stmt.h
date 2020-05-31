
#ifndef LOX_INTERPRETER_STMT_H
#define LOX_INTERPRETER_STMT_H

#include <Token.h>
#include <memory>

class Expr;
class VisitorStmt;

class Stmt {
public:
    virtual void accept(VisitorStmt& visitor) = 0;
    virtual ~Stmt() = default;
};

class ExprStmt: public Stmt {
public:
    std::unique_ptr<Expr> expr;
    explicit ExprStmt(std::unique_ptr<Expr> expr): expr(std::move(expr)) { };
    void accept(VisitorStmt& visitor) override;
};

class PrintStmt: public Stmt {
public:
    std::unique_ptr<Expr> expr;
    explicit PrintStmt(std::unique_ptr<Expr> expr): expr(std::move(expr)) { };
    void accept(VisitorStmt& visitor) override;
};

class VarDeclStmt: public Stmt {
public:
    Token varToken;
    std::unique_ptr<Expr> expr;
    explicit VarDeclStmt(Token varToken, std::unique_ptr<Expr> expr):
        varToken(std::move(varToken)), expr(std::move(expr)) { };
    void accept(VisitorStmt& visitor) override;
};

#endif //LOX_INTERPRETER_STMT_H
