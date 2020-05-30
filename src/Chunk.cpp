#include <Chunk.h>
#include <cassert>
#include <OpCode.h>
#include <cstdlib>
#include <stdexcept>
#include <Token.h>
#include <Error.h>
#include <Object.h>

void Chunk::emitByte(uint8_t byte, int line) {
    // e.g. OpCode with no argument
    code.push_back(byte);
    lines.push_back(line);
    assert(code.size() == lines.size());
}


void Chunk::emitBytes(uint8_t first, uint8_t second, int line) {
    // e.g. OpCode with one argument
    emitByte(first, line);
    emitByte(second, line);
}

uint8_t Chunk::addConstant(Value value) {
    constants.push_back(value);
    return constants.size() - 1;
}

void Chunk::emitConstantValue(const Token& token) {
    Value constant;
    switch (token.type) {
        case TOKEN_NUMBER:
            constant = atof(token.lexeme.c_str());
            break;
        case TOKEN_STRING:
            constant = StringPool::getInstance().getStringObj(token.lexeme);
            break;
        default:
            throw std::logic_error("unexpected token type [Chunk.cpp]");
    }
    auto constant_id = addConstant(constant);
    if(constant_id > 255) {
        throw CompileError(token, "more than 255 constants in one chunk");
    }
    emitBytes(static_cast<uint8_t>(OpCode::OP_CONSTANT), constant_id, token.line);
}

void Chunk::emitOpCode(OpCode opCode, int line) {
    emitByte(static_cast<uint8_t>(opCode), line);
}

void Chunk::emitOpCodes(OpCode first, OpCode second, int line) {
    emitByte(static_cast<uint8_t>(first), line);
    emitByte(static_cast<uint8_t>(second), line);
}


