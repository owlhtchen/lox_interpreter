#include <Chunk.h>
#include <cassert>

void Chunk::emitByte(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
    assert(code.size() == lines.size());
}

uint8_t Chunk::addConstant(Value value) {
    constants.push_back(value);
    if(constants.size() > 256) {
        // uint8_t allows only 256 integers
        
    }
    return constants.size() - 1;
}
