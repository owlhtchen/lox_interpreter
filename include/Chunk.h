
#ifndef LOX_INTERPRETER_CHUNK_H
#define LOX_INTERPRETER_CHUNK_H

#include <vector>
#include <Value.h>
#include <cstdint>
#include <Token.h>
#include <cstdint>
#include <OpCode.h>

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
};

#endif //LOX_INTERPRETER_CHUNK_H
