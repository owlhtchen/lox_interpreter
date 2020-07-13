#include <CodeGenerator.h>
#include <OpCode.h>
#include <Expr.h>
#include <Error.h>
#include <stdexcept>
#include <FunctionCompiler.h>
#include <DerivedObject.h>
#include <Stmt.h>
#include <DerivedObject.h>
#include <iostream>
#include <FunctionType.h>

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
        case TOKEN_NUMBER:
        case TOKEN_STRING:
            chunk->emitConstantValue(expr.token);
            break;
        case TOKEN_IDENTIFIER: {
            int localIndex = currentCompiler->resolveLocal(expr.token);
            if(localIndex >= 0) { // local variable
                chunk->emitOpCodeByte(OpCode::OP_GET_LOCAL, localIndex, line);
                break;
            }
            int upValueIndex = currentCompiler->resolveUpValue(expr.token);
            if(upValueIndex >= 0) { // variable is captured in closure
                chunk->emitOpCodeByte(OpCode::OP_GET_UPVALUE, upValueIndex, line);
                break;
            }
            // global variable
            StringObj* varName = StringPool::getInstance().getStringObj(expr.token.lexeme);
            uint8_t globalIndex = chunk->addConstant(varName, line);
            chunk->emitOpCodeByte(OpCode::OP_GET_GLOBAL, globalIndex, line);
            break;
        }
        default:
            throw std::logic_error("unhandled literal expr type in visitLiteralExpr");
    }
}

void CodeGenerator::visitUnaryExpr(const UnaryExpr &expr) {
    auto chunk = getCurrentChunk();
    compileExpr(*expr.right);
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
    compileExpr(*expr.innerExpr);
}

void CodeGenerator::visitBinaryExpr(const BinaryExpr &expr) {
    auto chunk = getCurrentChunk();
    compileExpr(*expr.left);
    compileExpr(*expr.right);
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

void CodeGenerator::compileExpr(const Expr &expr) {
    expr.accept(*this);
}

Chunk *CodeGenerator::getCurrentChunk() {
    return &currentCompiler->functionObj->chunk;
}

void CodeGenerator::visitExprStmt(const ExprStmt & stmt) {
    compileExpr(*stmt.expr);
    getCurrentChunk()->emitOpCode(OpCode::OP_POP, stmt.line);
}

void CodeGenerator::visitPrintStmt(const PrintStmt & stmt) {
    compileExpr(*stmt.expr);
    getCurrentChunk()->emitOpCode(OpCode::OP_PRINT, stmt.line);
}

void CodeGenerator::visitVarDeclStmt(const VarDeclStmt &stmt) {
    auto index = declareVariable(stmt.varToken);

    // initial value of variable is loaded onto the stack
    if(stmt.expr) {
        stmt.expr->accept(*this);
    } else {
        // default value null
        getCurrentChunk()->emitOpCode(OpCode::OP_NIL, stmt.varToken.line);
    }

    defineVariable(index, stmt.varToken.line);
}

uint8_t CodeGenerator::declareVariable(const Token& token) {
    // declare var: 1. global (stored variable name in a table/map): do nothing,
    // 2. local (stored on stack): addLocal to currentCompiler->locals (depth -1)
    if(currentCompiler->currentScopeDepth > 0) {
        // declare local variable in currentCompiler->locals
        currentCompiler->declareLocal(token);
        return 0;
    } else {
        StringObj* stringObj = StringPool::getInstance().getStringObj(token.lexeme);
        return getCurrentChunk()->addConstant(stringObj, token.line);
    }
}

void CodeGenerator::defineVariable(uint8_t varNameIndex, int line) {
    // 1. global: emit OpCode and varIdentifier_index in constants (to set global map)
    // 2. local: just leave the init_expression value on the stack
    // (set depth in currentCompiler->locals (markLocalDefined))
    if(currentCompiler->currentScopeDepth != 0) {
        currentCompiler->markLocalDefined();
    } else {
        defineGlobal(varNameIndex, line);
    }
}

void CodeGenerator::defineGlobal(uint8_t varIdentifierId, int line) {
    if(currentCompiler->currentScopeDepth > 0) {
        return;
    }
    getCurrentChunk()->emitOpCodeByte(OpCode::OP_DEFINE_GLOBAL, varIdentifierId, line);
}

void CodeGenerator::visitBlockStmt(const BlockStmt &stmt) {
    currentCompiler->beginScope();
    for(const auto & statement : stmt.statements) {
        statement->accept(*this);
    }
    currentCompiler->endScope(stmt.end);
}

void CodeGenerator::compileStmt(const Stmt &stmt) {
    stmt.accept(*this);
}

FunctionObj *CodeGenerator::compile(const std::vector<std::unique_ptr<Stmt>> &statements) {
    currentCompiler = std::make_shared<FunctionCompiler>(nullptr, SCRIPT_TYPE, "");
    for(const auto& stmt: statements) {
        stmt->accept(*this);
    }
    int lastLine = statements.empty()? 0 : statements.back()->getLastLine();
    return endCurrentCompiler(lastLine);
}

FunctionObj *CodeGenerator::endCurrentCompiler(int line) {
    // there might be a returnStmt in code and OP_RETURN is emitted, but emitting twice doesn't hurt
    // since the CallFrame will be popped when the first OP_RETURN is seen
    auto chunk = getCurrentChunk();
    chunk->emitByte(static_cast<uint8_t>(OpCode::OP_NIL), line);
    chunk->emitByte(static_cast<uint8_t>(OpCode::OP_RETURN), line);
    currentCompiler->functionObj->closureCount = currentCompiler->upValues.size();
    auto temp = currentCompiler->functionObj;
    currentCompiler = currentCompiler->enclosing;
    return temp;
}

void CodeGenerator::visitAssignExpr(const AssignExpr &expr) {
    int index = currentCompiler->resolveLocal(expr.variable);
    int line = expr.getLastLine();
    auto chunk = getCurrentChunk();

    compileExpr(*expr.assignValue); // assignValue is put on stackTop

    if(index >= 0) { // local variable
        chunk->emitOpCodeByte(OpCode::OP_SET_LOCAL, index, line);
        return;
    }
    index = currentCompiler->resolveUpValue(expr.variable);
    if(index >= 0) { // upValue captured in closure
        chunk->emitOpCodeByte(OpCode::OP_SET_UPVALUE, index, line);
        return;
    }
    // global variable
    Value varName = StringPool::getInstance().getStringObj(expr.variable.lexeme);
    index = chunk->addConstant(varName, expr.getLastLine());
    chunk->emitOpCodeByte(OpCode::OP_SET_GLOBAL, index, line);
}

void CodeGenerator::visitFunctionStmt(const FunctionStmt& functionStmt) {
    int funcNameIndex = declareVariable(functionStmt.funcName);
    int funcNameLine = functionStmt.funcName.line;
    std::string funcName = functionStmt.funcName.lexeme;
    std::cout << "visiting " << funcName << std::endl;
    currentCompiler->markLocalDefined();

    // functionObj will be loaded on stack top during runtime
    currentCompiler = std::make_shared<FunctionCompiler>(currentCompiler, FUNCTION_TYPE, funcName);
    currentCompiler->beginScope();
    for(const auto& param: functionStmt.params) {
        declareVariable(param);
        currentCompiler->markLocalDefined();
        currentCompiler->functionObj->arity++;
    }
    auto & stmts = functionStmt.body;
    for(const auto& stmt: stmts) {
        stmt->accept(*this);
    }
    int lastLine = stmts.empty()? 0 : stmts.back()->getLastLine();
    FunctionObj* functionObj = endCurrentCompiler(lastLine);
//    FunctionObj* functionObj = currentCompiler->functionObj;
    auto chunk = getCurrentChunk();
    auto index = chunk->addConstant(functionObj, funcNameLine);
//    chunk->emitOpCodeByte(OpCode::OP_CONSTANT, index, funcNameLine);
    chunk->emitOpCodeByte(OpCode::OP_CLOSURE, index, funcNameLine);
    // these should be before endCurrentCompiler!!
    std::cout << " current upvalue.size(): " << currentCompiler->upValues.size() << " for " << currentCompiler->functionObj->getName() << std::endl;
    for(const auto & upValue: currentCompiler->upValues) {
        std::cout << "emitted index: " << std::to_string(upValue.index)
        << " , local: " << std::to_string(upValue.isLocal) << std::endl;
        chunk->emitByte(upValue.index, funcNameLine);
        chunk->emitByte(upValue.isLocal ? 1 : 0, funcNameLine);
    }
//     endCurrentCompiler(lastLine);
    std::cout << "end visiting " << funcName << std::endl;

    defineVariable(funcNameIndex, funcNameLine);
}

void CodeGenerator::visitCallExpr(const CallExpr &callExpr) {
    compileExpr(*callExpr.callee); // push callee (as functionObj in Value) onto stack
    int actualArity = 0;
    for(const auto& argument: callExpr.arguments) {
        // push arguments onto stack
        compileExpr(*argument);
        actualArity++;
    }
    auto chunk = getCurrentChunk();
    chunk->emitOpCodeByte(OpCode::OP_CALL, actualArity, callExpr.line);
}

void CodeGenerator::visitReturnStmt(const ReturnStmt &returnStmt) {
    // TODO: check function type
    if(currentCompiler->functionType == SCRIPT_TYPE) {
        throw CompileError(returnStmt.getLastLine(), "'return' outside function");
    }
    auto chunk = getCurrentChunk();
    int line = returnStmt.getLastLine();
    if(returnStmt.returnExpr) {
        compileExpr(*returnStmt.returnExpr);
    } else {
        chunk->emitByte(static_cast<uint8_t>(OpCode::OP_NIL), line);
    }
    chunk->emitByte(static_cast<uint8_t>(OpCode::OP_RETURN), line);
}

