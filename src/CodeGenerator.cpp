#include <CodeGenerator.h>
#include <OpCode.h>
#include <Expr.h>
#include <Error.h>
#include <stdexcept>
#include <Compiler.h>
#include <Object.h>
#include <Stmt.h>
#include <Object.h>

void CodeGenerator::visitLiteralExpr(const LiteralExpr &expr) {
    auto line = expr.token.line;
    auto chunk = getCurrentChunk();
    switch (expr.token.type) {
        case TOKEN_TRUE:
            chunk->emitOpCode(OpCode::OP_TRUE, line);
            break;
        case TOKEN_FALSE:
            chunk->emitOpCode(OpCode::OP_FALSE, line);
            break;
        case TOKEN_NIL:
            chunk->emitOpCode(OpCode::OP_NIL, line);
            break;
        case TOKEN_STRING:
        case TOKEN_NUMBER:
            chunk->emitConstantValue(expr.token);
            break;
        default:
            throw std::logic_error("unhandled literal expr type in visitLiteralExpr");
    }
}

void CodeGenerator::visitUnaryExpr(const UnaryExpr &expr) {
    auto chunk = getCurrentChunk();
    compile(*expr.right);
    switch (expr.opr.type) {
        case TOKEN_MINUS:
            chunk->emitOpCode(OpCode::OP_NEGATE, expr.opr.line);
            break;
        case TOKEN_BANG:
            chunk->emitOpCode(OpCode::OP_NOT, expr.opr.line);
            break;
        default:
            throw CompileError(expr.opr, "unexpected unary expr token " + expr.opr.lexeme);
    }
}

void CodeGenerator::visitGroupingExpr(const GroupingExpr &expr) {
    compile(*expr.innerExpr);
}

void CodeGenerator::visitBinaryExpr(const BinaryExpr &expr) {
    auto chunk = getCurrentChunk();
    compile(*expr.left);
    compile(*expr.right);
    switch (expr.opr.type) {
        case TOKEN_PLUS:
            chunk->emitOpCode(OpCode::OP_ADD, expr.opr.line);
            break;
        case TOKEN_MINUS:
            chunk->emitOpCode(OpCode::OP_SUBTRACT, expr.opr.line);
            break;
        case TOKEN_STAR:
            chunk->emitOpCode(OpCode::OP_MULTIPLY, expr.opr.line);
            break;
        case TOKEN_SLASH:
            chunk->emitOpCode(OpCode::OP_DIVIDE, expr.opr.line);
            break;
        case TOKEN_EQUAL_EQUAL:
            chunk->emitOpCode(OpCode::OP_EQUAL, expr.opr.line);
            break;
        case TOKEN_GREATER:
            chunk->emitOpCode(OpCode::OP_GREATER, expr.opr.line);
            break;
        case TOKEN_LESS:
            chunk->emitOpCode(OpCode::OP_LESS, expr.opr.line);
            break;
        case TOKEN_BANG_EQUAL:
            chunk->emitOpCodes(OpCode::OP_EQUAL, OpCode::OP_NOT, expr.opr.line);
            break;
        case TOKEN_GREATER_EQUAL:
            chunk->emitOpCodes(OpCode::OP_LESS, OpCode::OP_NOT, expr.opr.line);
            break;
        case TOKEN_LESS_EQUAL:
            chunk->emitOpCodes(OpCode::OP_GREATER, OpCode::OP_NOT, expr.opr.line);
            break;
        default:
            throw CompileError(expr.opr, "unexpected binary expr token " + expr.opr.lexeme);
    }
}

void CodeGenerator::compile(const Expr &expr) {
    expr.accept(*this);
}

Chunk *CodeGenerator::getCurrentChunk() {
    return &currentCompiler->functionObj->chunk;
}

void CodeGenerator::visitExprStmt(const ExprStmt & stmt) {
    compile(*stmt.expr);
    getCurrentChunk()->emitOpCode(OpCode::OP_POP, stmt.line);
}

void CodeGenerator::visitPrintStmt(const PrintStmt & stmt) {
    compile(*stmt.expr);
    getCurrentChunk()->emitOpCode(OpCode::OP_PRINT, stmt.line);
}

void CodeGenerator::visitVarDeclStmt(const VarDeclStmt &stmt) {
    // declare var: 1. global (stored in a table/map): do nothing,
    // 2. local (stored on stack): addLocal to currentCompiler->locals (depth -1)
    auto index = parseVariable(stmt.varToken);

    if(stmt.expr) {
        stmt.expr->accept(*this);
    } else {
        getCurrentChunk()->emitOpCode(OpCode::OP_NIL, stmt.varToken.line);
    }
    // define var: default value null,
    // 1. global: emit OpCode and varIdentifier_index in constants (to set global map)
    // 2. local: just leave the init_expression value on the stack
    // (set depth in currentCompiler->locals (markDefined))
    if(currentCompiler->currentScopeDepth == 0) {
        currentCompiler->markDefined();
    } else {
        defineGlobal(index, stmt.varToken.line);
    }
}

uint8_t CodeGenerator::parseVariable(const Token& token) {
    if(currentCompiler->currentScopeDepth == 0) {
        // declare local variable in currentCompiler->locals
        currentCompiler->declareLocal(token);
        return 0;
    } else {
        StringObj* stringObj = StringPool::getInstance().getStringObj(token.lexeme);
        return getCurrentChunk()->addConstant(stringObj);
    }
}

void CodeGenerator::defineGlobal(uint8_t varIdentifierId, int line) {
    if(currentCompiler->currentScopeDepth > 0) {
        return;
    }
    getCurrentChunk()->emitOpCodeByte(OpCode::OP_SET_GLOBAL, varIdentifierId, line);
}

void CodeGenerator::visitBlockStmt(const BlockStmt &stmt) {
    currentCompiler->beginScope();
    for(const auto & statement : stmt.statements) {
        statement->accept(*this);
    }
    currentCompiler->endScope();
}


