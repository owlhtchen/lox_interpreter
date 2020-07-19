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
#include <memory>

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
            getVariable(expr.token);
            break;
        }
        case TOKEN_THIS: {
            getVariable(expr.token);
            break;
        }
        default:
            throw std::logic_error("unhandled literal expr type in visitLiteralExpr");
    }
}

void CodeGenerator::getVariable(const Token& token) {
    auto chunk = getCurrentChunk();
    int line = token.line;
    int localIndex = currentCompiler->resolveLocal(token);
    if(localIndex >= 0) { // local variable
        chunk->emitOpCodeByte(OpCode::OP_GET_LOCAL, localIndex, line);
        return;
    }
    int upValueIndex = currentCompiler->resolveUpValue(token);
    if(upValueIndex >= 0) { // variable is captured in closure
        chunk->emitOpCodeByte(OpCode::OP_GET_UPVALUE, upValueIndex, line);
        return;
    }
    // global variable
    StringObj* varName = StringPool::getInstance().getStringObj(token.lexeme);
    uint8_t globalIndex = chunk->addConstant(varName, line);
    chunk->emitOpCodeByte(OpCode::OP_GET_GLOBAL, globalIndex, line);
    return;
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
    if(currentCompiler->functionType != CONSTRUCTOR_TYPE) {
        chunk->emitByte(static_cast<uint8_t>(OpCode::OP_NIL), line);
    } else {
        Token _this = Token(TOKEN_THIS, "this", line);
        getVariable(_this);
    }
    chunk->emitByte(static_cast<uint8_t>(OpCode::OP_RETURN), line);
    currentCompiler->functionObj->closureCount = currentCompiler->upValues.size();
    auto tmp = currentCompiler->functionObj;
    currentCompiler = currentCompiler->enclosing;
    return tmp;
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

void CodeGenerator::compileFunctionStmt(const FunctionStmt & functionStmt, FunctionType functionType) {
    std::string funcName = functionStmt.funcName.lexeme;
    int funcNameLine = functionStmt.funcName.line;

    // functionObj will be loaded on stack top during runtime
    currentCompiler = std::make_shared<FunctionCompiler>(currentCompiler, functionType, funcName);
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
    std::shared_ptr<FunctionCompiler> newFunctionCompiler = currentCompiler;
    FunctionObj* functionObj = endCurrentCompiler(lastLine);
    auto chunk = getCurrentChunk();
    auto index = chunk->addConstant(functionObj, funcNameLine);
//    chunk->emitOpCodeByte(OpCode::OP_CONSTANT, index, funcNameLine);
    chunk->emitOpCodeByte(OpCode::OP_CLOSURE, index, funcNameLine);
    // newFunctionCompiler.enclosing == currentCompiler
    for(const auto & upValue: newFunctionCompiler->upValues) {
        chunk->emitByte(upValue.index, funcNameLine);
        chunk->emitByte(upValue.isLocal ? 1 : 0, funcNameLine);
    }
}

void CodeGenerator::visitFunctionStmt(const FunctionStmt& functionStmt) {
    int funcNameIndex = declareVariable(functionStmt.funcName);
    int funcNameLine = functionStmt.funcName.line;
    currentCompiler->markLocalDefined();

    compileFunctionStmt(functionStmt, FUNCTION_TYPE);

    defineVariable(funcNameIndex, funcNameLine);

    /*
    should NOT call endScope after visitFunctionStmt
    because op return already pops all remaining local variable from the stack
    and op return should deal with capturing local variable as closed upvalue (instead of popping)
     // currentCompiler->endScope(functionStmt.getLastLine());
     * */
}

int CodeGenerator::compileArguments(const std::vector<std::unique_ptr<Expr>> & arguments) {
    int actualArity = 0;
    for(const auto& argument: arguments) {
        // push arguments onto stack
        compileExpr(*argument);
        actualArity++;
    }
    return actualArity;
}

void CodeGenerator::visitCallExpr(const CallExpr &callExpr) {
    int line = callExpr.getLastLine();
    auto chunk = getCurrentChunk();
    auto getPtr = callExpr.callee.get();
    auto calleeGetExpr = dynamic_cast<GetExpr*>(getPtr);
    auto superPtr = callExpr.callee.get();
    auto calleeSuperExpr = dynamic_cast<SuperExpr*>(superPtr);

    if(calleeGetExpr != nullptr) { // [optimized] e.g. object.method(arguments)
        compileExpr(*calleeGetExpr->object);  // push object on stack
        StringObj* method = StringPool::getInstance().getStringObj(calleeGetExpr->field.lexeme);
        auto methodName = chunk->addConstant(method, line);
        int actualArity = compileArguments(callExpr.arguments); // push arguments on stack
        chunk->emitOpCodeByte(OpCode::OP_INVOKE, methodName, line);
        chunk->emitByte(actualArity, line);
    } else if(calleeSuperExpr != nullptr) { // [optimized] e.g. super.superclassMethod(arguments)
        Token superToken(TOKEN_THIS, "this", line);
        getVariable(superToken);
        int actualArity = compileArguments(callExpr.arguments); // push arguments on stack
        compileSuperclassField(calleeSuperExpr->identifier.lexeme, OpCode::OP_SUPER_INVOKE, line);
        chunk->emitByte(actualArity, line);
    } else { // e.g. var func = object.method; func()
            compileExpr(*callExpr.callee); // push callee (e.g. as closureObj in Value) onto stack
            int actualArity = compileArguments(callExpr.arguments);
            chunk->emitOpCodeByte(OpCode::OP_CALL, actualArity, line);
    }

}

void CodeGenerator::visitReturnStmt(const ReturnStmt &returnStmt) {
    if(currentCompiler->functionType == SCRIPT_TYPE) {
        throw CompileError(returnStmt.getLastLine(), "'return' outside function");
    }
    auto chunk = getCurrentChunk();
    int line = returnStmt.getLastLine();
    if(returnStmt.returnExpr) {
        if(currentCompiler->functionType == CONSTRUCTOR_TYPE) {
            throw CompileError(line, "constructor cannot have a return value");
        }
        compileExpr(*returnStmt.returnExpr);
    } else {
        if(currentCompiler->functionType == CONSTRUCTOR_TYPE) {
            Token _this = Token(TOKEN_THIS, "this", line);
            getVariable(_this);
        } else {
            chunk->emitByte(static_cast<uint8_t>(OpCode::OP_NIL), line);
        }
    }
    chunk->emitByte(static_cast<uint8_t>(OpCode::OP_RETURN), line);
}

void CodeGenerator::visitClassStmt(const ClassStmt &classStmt) {
    // define class
    auto index = declareVariable(classStmt.name);
    int lastLine = classStmt.getLastLine();
    int classNameLine = classStmt.name.line;

    auto chunk = getCurrentChunk();
    chunk->emitOpCodeByte(OpCode::OP_CLASS, index, classStmt.name.line);

    defineVariable(index, classStmt.getLastLine());

    if(classStmt.hasSuperclass) {    // define super as local variable, super = superclass
        currentCompiler->beginScope();
        Token superToken(TOKEN_SUPER, "super", classNameLine);
        auto superIndex = declareVariable(superToken);
        getVariable(classStmt.superclass);
        defineVariable(superIndex, classNameLine);
    }

    // define methods of the class
    getVariable(classStmt.name);  // push classObj onto stack
    if(classStmt.hasSuperclass) {  // inherit methods from superclass
        getVariable(classStmt.superclass);
        chunk->emitOpCode(OpCode::OP_INHERIT, lastLine);
    }
    for(const auto & method: classStmt.methods) {
        if(method->funcName.lexeme == "init") {
            compileFunctionStmt(*method, CONSTRUCTOR_TYPE);
        } else {
            compileFunctionStmt(*method, METHOD_TYPE);
        }
        StringObj* name = StringPool::getInstance().getStringObj(method->funcName.lexeme);
        int methodLine = method->getLastLine();
        auto methodName = chunk->addConstant(name, methodLine);
        chunk->emitOpCodeByte(OpCode::OP_METHOD, methodName, methodLine);
    }
    chunk->emitOpCode(OpCode::OP_POP, lastLine); // pop classObj off stack

    if(classStmt.hasSuperclass) {
        currentCompiler->endScope(lastLine);
    }
}

void CodeGenerator::visitGetExpr(const GetExpr &getExpr) {
    compileExpr(*getExpr.object);
    auto chunk = getCurrentChunk();
    int line = getExpr.getLastLine();
    StringObj* name = StringPool::getInstance().getStringObj(getExpr.field.lexeme);
    auto fieldName = chunk->addConstant(name, line);
    chunk->emitOpCodeByte(OpCode::OP_GET_PROPERTY, fieldName, line);
}


void CodeGenerator::visitSetExpr(const SetExpr &setExpr) {
    int line = setExpr.getLastLine();
    // push new value on stack
    compileExpr(*setExpr.value);
    // push object on stack
    compileExpr(*setExpr.object);
    auto chunk = getCurrentChunk();
    StringObj* name = StringPool::getInstance().getStringObj(setExpr.field.lexeme);
    auto fieldName = chunk->addConstant(name, line);
    chunk->emitOpCodeByte(OpCode::OP_SET_PROPERTY, fieldName, line);
}

void CodeGenerator::visitSuperExpr(const SuperExpr & superExpr) {
    int line = superExpr.line;
    compileSuperclassField(superExpr.identifier.lexeme, OpCode::OP_GET_SUPER, line);
}

void CodeGenerator::compileSuperclassField(const std::string& superclassFieldName, OpCode opCode, int line) {
    auto chunk = getCurrentChunk();
    Token superToken(TOKEN_SUPER, "super", line);
    getVariable(superToken);  // push superclass onto stack
    StringObj* method = StringPool::getInstance().getStringObj(superclassFieldName);
    auto methodName = chunk->addConstant(method, line);
    chunk->emitOpCodeByte(opCode, methodName, line);
}

void CodeGenerator::visitIfStmt(const IfStmt &stmt) {
    compileExpr(*stmt.condition);  // condition evaluates to true/false and it's on stack top
    int condLine = stmt.condition->getLastLine();
    int thenLine = stmt.thenStmt->getLastLine();
    auto chunk = getCurrentChunk();
    int toElse = chunk->emitJump(OpCode::OP_JUMP_IF_FALSE, condLine);
    chunk->emitOpCode(OpCode::OP_POP, condLine);
    compileStmt(*stmt.thenStmt);
    int toEnd = chunk->emitJump(OpCode::OP_JUMP, thenLine);
    chunk->patchJumpOffset(toElse, chunk->getCodeSize() - (toElse + 2));
    if(stmt.elseStmt) {
        compileStmt(*stmt.elseStmt);
    }
    chunk->patchJumpOffset(toEnd, chunk->getCodeSize() - (toEnd + 2));
}

void CodeGenerator::visitWhileStmt(const WhileStmt& stmt) {
    auto chunk = getCurrentChunk();
    int condLine = stmt.condition->getLastLine();
    int bodyLine = stmt.body->getLastLine();
    int whileStart = chunk->getCodeSize();
    currentCompiler->beginScope();
    compileExpr(*stmt.condition);
    int toEnd = chunk->emitJump(OpCode::OP_JUMP_IF_FALSE, condLine);
    chunk->emitOpCode(OpCode::OP_POP, condLine);
    compileStmt(*stmt.body);
    int toStart = chunk->emitJump(OpCode::OP_LOOP, bodyLine);
    chunk->patchJumpOffset(toStart, toStart + 2 - whileStart);
    chunk->patchJumpOffset(toEnd, chunk->getCodeSize() - (toEnd + 2));
    currentCompiler->endScope(bodyLine);
}

void CodeGenerator::visitForStmt(const ForStmt &stmt) {
    int line = stmt.getLastLine();
    auto chunk = getCurrentChunk();
    currentCompiler->beginScope();
    if(stmt.initializer) {
        compileStmt(*stmt.initializer);
    }
    int whileStart = chunk->getCodeSize();
    if(stmt.condition) {
        compileExpr(*stmt.condition);
    }
    int toEnd = chunk->emitJump(OpCode::OP_JUMP_IF_FALSE, line);
    chunk->emitOpCode(OpCode::OP_POP, line);
    compileStmt(*stmt.body);
    if(stmt.increment) {
        compileExpr(*stmt.increment);
    }
    int toStart = chunk->emitJump(OpCode::OP_LOOP, line);
    chunk->patchJumpOffset(toStart, toStart + 2 - whileStart);
    chunk->patchJumpOffset(toEnd, chunk->getCodeSize() - (toEnd + 2));
    currentCompiler->endScope(line);
}

void CodeGenerator::visitLogicalExpr(const LogicalExpr &expr) {
    auto chunk = getCurrentChunk();
    int line = expr.getLastLine();
    compileExpr(*expr.left);
    int logicalLeft;
    if(expr.opr.type == TOKEN_AND) { // and short circuit
        logicalLeft = chunk->emitJump(OpCode::OP_JUMP_IF_FALSE, line);
    } else { // or short circuit
        logicalLeft = chunk->emitJump(OpCode::OP_JUMP_IF_TRUE, line);
    }
    chunk->emitOpCode(OpCode::OP_POP, line);
    compileExpr(*expr.right);
    chunk->patchJumpOffset(logicalLeft, chunk->getCodeSize() - (logicalLeft + 2));
}
