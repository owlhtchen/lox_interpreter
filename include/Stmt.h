
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
    virtual int getLastLine() const = 0;
};

class ExprStmt: public Stmt {
public:
    std::unique_ptr<Expr> expr;
    int line;
    explicit ExprStmt(std::unique_ptr<Expr> expr, int line): expr(std::move(expr)), line(line) { };
    void accept(VisitorStmt& visitor) const override;
    int getLastLine() const override;
};

class PrintStmt: public Stmt {
public:
    std::unique_ptr<Expr> expr;
    int line;
    explicit PrintStmt(std::unique_ptr<Expr> expr, int line): expr(std::move(expr)), line(line) { };
    void accept(VisitorStmt& visitor) const override;
    int getLastLine() const override;
};

class VarDeclStmt: public Stmt {
public:
    Token varToken;
    std::unique_ptr<Expr> expr;
    explicit VarDeclStmt(Token varToken, std::unique_ptr<Expr> expr):
        varToken(std::move(varToken)), expr(std::move(expr)) { };
    void accept(VisitorStmt& visitor) const override;
    int getLastLine() const override;
};

class BlockStmt: public Stmt {
public:
    int begin, end;
    std::vector<std::unique_ptr<Stmt>> statements;
    void accept(VisitorStmt& visitor) const override;
    int getLastLine() const override;
};

class FunctionStmt: public Stmt {
public:
    Token funcName;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
    FunctionStmt(Token funcName, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body):
        funcName(std::move(funcName)), params(std::move(params)), body(std::move(body)) { };
    void accept(VisitorStmt& visitor) const override ;
    int getLastLine() const override ;
};

class ReturnStmt: public Stmt {
public:
    Token returnKeyword;
    std::unique_ptr<Expr> returnExpr;
    ReturnStmt(Token returnKeyword, std::unique_ptr<Expr> returnExpr):
        returnKeyword(std::move(returnKeyword)), returnExpr(std::move(returnExpr)) { };
    void accept (VisitorStmt& visitor) const override ;
    int getLastLine() const override ;
};

class ClassStmt: public Stmt {
public:
    Token name;
    bool hasSuperclass;
    Token superclass;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    int lastLine;
    ClassStmt(Token name, std::vector<std::unique_ptr<FunctionStmt>> methods, int lastLine):
        name(std::move(name)), methods(std::move(methods)), lastLine(lastLine), hasSuperclass(false) { };
    void accept(VisitorStmt& visitor) const override ;
    int getLastLine() const override ;
    void setSuperclass(Token superclass);
};

#endif //LOX_INTERPRETER_STMT_H
