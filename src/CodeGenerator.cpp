#include <CodeGenerator.h>
#include <OpCode.h>
#include <Expr.h>
#include <CompileError.h>

void CodeGenerator::visitLiteralExpr(const LiteralExpr &expr) {
    chunk.emitConstantValue(expr.token);
}

void CodeGenerator::visitUnaryExpr(const UnaryExpr &expr) {
    compile(*expr.right);
    switch (expr.opr.type) {
        case TOKEN_MINUS:
            chunk.emitOpCode(OpCode::OP_NEGATE, expr.opr.line);
            break;
        default:
            throw CompileError(expr.opr, "unexpected unary expr token " + expr.opr.lexeme);
    }
}

void CodeGenerator::visitGroupingExpr(const GroupingExpr &expr) {
    compile(*expr.innerExpr);
}

void CodeGenerator::visitBinaryExpr(const BinaryExpr &expr) {
    compile(*expr.left);
    compile(*expr.right);
    switch (expr.opr.type) {
        case TOKEN_PLUS:
            chunk.emitOpCode(OpCode::OP_ADD, expr.opr.line);
            break;
        case TOKEN_MINUS:
            chunk.emitOpCode(OpCode::OP_SUBTRACT, expr.opr.line);
            break;
        case TOKEN_STAR:
            chunk.emitOpCode(OpCode::OP_MULTIPLY, expr.opr.line);
            break;
        case TOKEN_SLASH:
            chunk.emitOpCode(OpCode::OP_DIVIDE, expr.opr.line);
            break;
        default:
            throw CompileError(expr.opr, "unexpected binary expr token " + expr.opr.lexeme);
    }
}

void CodeGenerator::compile(Expr &expr) {
    expr.accept(*this);
}
