
#ifndef LOX_INTERPRETER_STMT_H
#define LOX_INTERPRETER_STMT_H

#include <Token.h>
#include <memory>
#include <vector>

class Expr;
class VisitorStmt;

class Stmt {
public:
    virtual void accept(VisitorStmt& visitor) const = 0;
    virtual ~Stmt() = default;
};

class ExprStmt: public Stmt {
public:
    std::unique_ptr<Expr> expr;
    int line;
    explicit ExprStmt(std::unique_ptr<Expr> expr, int line): expr(std::move(expr)), line(line) { };
    void accept(VisitorStmt& visitor) const override;
};

class PrintStmt: public Stmt {
public:
    std::unique_ptr<Expr> expr;
    int line;
    explicit PrintStmt(std::unique_ptr<Expr> expr, int line): expr(std::move(expr)), line(line) { };
    void accept(VisitorStmt& visitor) const override;
};

class VarDeclStmt: public Stmt {
public:
    Token varToken;
    std::unique_ptr<Expr> expr;
    explicit VarDeclStmt(Token varToken, std::unique_ptr<Expr> expr):
        varToken(std::move(varToken)), expr(std::move(expr)) { };
    void accept(VisitorStmt& visitor) const override;
};

class BlockStmt: public Stmt {
public:
    int begin, end;
    std::vector<std::unique_ptr<Stmt>> statements;
    void accept(VisitorStmt& visitor) const override;
};

#endif //LOX_INTERPRETER_STMT_H
