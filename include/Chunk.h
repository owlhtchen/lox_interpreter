
#ifndef LOX_INTERPRETER_CHUNK_H
#define LOX_INTERPRETER_CHUNK_H

#include <vector>
#include <Value.h>
#include <cstdint>
#include <Token.h>
#include <cstdint>

// using ValueArray = std::vector<Value>;

class Chunk{
private:
    std::vector<uint8_t> code;
    std::vector<int> lines;
    std::vector<Value> constants;
    uint8_t addConstant(Value value);
public:
    void emitByte(uint8_t byte, int line);
};

#endif //LOX_INTERPRETER_CHUNK_H
