
#ifndef LOX_INTERPRETER_CHUNK_H
#define LOX_INTERPRETER_CHUNK_H

#include <vector>
#include <Value.h>
#include <cstdint>

using ValueArray = std::vector<Value>;

class Chunk{
private:
    std::vector<int> lines;
    std::vector<uint8_t> code;
    ValueArray constants;

public:
    void emitByte(uint8_t byte);
};

#endif //LOX_INTERPRETER_CHUNK_H
