
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

class VM;
class CallFrame;
// using ValueArray = std::vector<Value>;

class Chunk{
    friend class VM;
    friend class CallFrame;
private:
    std::vector<uint8_t> code;
    std::vector<int> lines;
    std::vector<Value> constants;
public:
    void emitByte(uint8_t byte, int line);
    void emitBytes(uint8_t first, uint8_t second, int line);
    uint8_t addConstant(Value value, int line);
    uint8_t emitConstantValue(const Token& token);
    void emitOpCode(OpCode opCode, int line);
    void emitOpCodes(OpCode first, OpCode second, int line);
    void emitOpCodeByte(OpCode opCode, uint8_t operand, int line);

#ifdef LOX_DEBUG
    friend void disassembleChunk(Chunk* chunk);
    friend int disassembleInstruction(Chunk* chunk, int i);
#endif
};

#endif //LOX_INTERPRETER_CHUNK_H
