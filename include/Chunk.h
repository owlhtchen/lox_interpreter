
#ifndef LOX_INTERPRETER_CHUNK_H
#define LOX_INTERPRETER_CHUNK_H

#include <vector>
#include <Value.h>
#include <cstdint>
#include <Token.h>
#include <cstdint>
#include <OpCode.h>
#include <common.h>

#ifdef LOX_DEBUG
#include <debug.h>
#endif

// using ValueArray = std::vector<Value>;

class Chunk{
private:
    std::vector<uint8_t> code;
    std::vector<int> lines;
    std::vector<Value> constants;
    uint8_t addConstant(Value value);
public:
    void emitByte(uint8_t byte, int line);
    void emitBytes(uint8_t first, uint8_t second, int line);
    void emitConstantValue(const Token& token);
    void emitOpCode(OpCode opCode, int line);


#ifdef LOX_DEBUG
    friend void disassembleChunk(Chunk* chunk);
    friend int disassembleInstruction(Chunk* chunk, int i);
#endif
};

#endif //LOX_INTERPRETER_CHUNK_H
