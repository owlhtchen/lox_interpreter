#include <Chunk.h>
#include <cassert>
#include <OpCode.h>
#include <cstdlib>
#include <stdexcept>
#include <Token.h>
#include <Error.h>
#include <DerivedObject.h>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <bitset>

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

uint8_t Chunk::addConstant(Value value, int line) {
    constants.push_back(value);
    auto constantId = constants.size() - 1;
    if(constantId > 255) {
        throw CompileError(line, "more than 255 constants in one chunk");
    }
    return constantId;
}

uint8_t Chunk::emitConstantValue(const Token& token) {
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
    auto constantId = addConstant(constant, token.line);
    emitBytes(static_cast<uint8_t>(OpCode::OP_CONSTANT), constantId, token.line);
    return constantId;
}

void Chunk::emitOpCode(OpCode opCode, int line) {
    emitByte(static_cast<uint8_t>(opCode), line);
}

void Chunk::emitOpCodes(OpCode first, OpCode second, int line) {
    emitByte(static_cast<uint8_t>(first), line);
    emitByte(static_cast<uint8_t>(second), line);
}

void Chunk::emitOpCodeByte(OpCode opCode, uint8_t operand, int line) {
    emitByte(static_cast<uint8_t>(opCode), line);
    emitByte(operand, line);
}

int Chunk::emitJump(OpCode opCode, int line) {
    emitOpCode(opCode, line);
    // placeholder
    emitByte(0, line);
    emitByte(0, line);
    return code.size() - 2;
}

void Chunk::patchJumpOffset(int startIp, uint16_t offset) {
    assert((offset >= 0 && offset < pow(2, 16)) && "jump offset out of range (0-65525)");
    std::bitset<8> high(offset >> 8);
    std::bitset<8> low(offset);
    code[startIp]  = high.to_ulong();
    code[startIp + 1] = low.to_ulong();
}

int Chunk::getCodeSize() {
    return code.size();
}