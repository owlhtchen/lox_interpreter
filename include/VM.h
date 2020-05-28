
#ifndef LOX_INTERPRETER_VM_H
#define LOX_INTERPRETER_VM_H

#include <vector>
#include <cstdint>
#include <Chunk.h>
#include <Value.h>
#include <OpCode.h>

class VM {
private:
    Chunk chunk;
    int ip;
    std::vector<Value> stack;
public:
    explicit VM(Chunk chunk): chunk(std::move(chunk)), ip(0) { };
    void run();
    Chunk& getCurrentChunk();
    uint8_t readByte();
    OpCode readOpCode();
    Value readConstant();
    Value peekStack(int relative_pos);
    int getCurrentLine();
    Value popStack();
};


#endif //LOX_INTERPRETER_VM_H
