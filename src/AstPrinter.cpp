
#include <iostream>
#include "Expr.h"
#include "AstPrinter.h"



void AstPrinter::printAst(Expr &expr) {
    expr.accept(*this);
}

void AstPrinter::visitLiteralExpr(const LiteralExpr &expr) {
    std::cout << expr.token.lexeme << " ";
}

void AstPrinter::visitUnaryExpr(const UnaryExpr &expr) {
    std::cout << expr.opr.lexeme;
    expr.right->accept(*this);
}

void AstPrinter::visitGroupingExpr(const GroupingExpr &expr) {
    std::cout << "(";
    expr.innerExpr->accept(*this);
    std::cout << ")";
}

void AstPrinter::visitBinaryExpr(const BinaryExpr &expr) {
    std::cout << expr.opr.lexeme;
    expr.left->accept(*this);
    expr.right->accept(*this);
}
