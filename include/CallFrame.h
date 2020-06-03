
#ifndef LOX_INTERPRETER_CALLFRAME_H
#define LOX_INTERPRETER_CALLFRAME_H

#include <Chunk.h>
#include <Value.h>
#include <OpCode.h>
#include <vector>
#include "VM.h"

class FunctionObj;
class VM;

class CallFrame {
private:
    FunctionObj* functionObj;
    int ip;
    int stackBase;
    //vm: a reference to the vm which contains this CallFrame
    VM & vm;
    // CallFrame can only be constructed in the conta
    CallFrame(FunctionObj* functionObj, int stackBase, VM& vm):
            functionObj(functionObj), stackBase(stackBase), ip(0), vm(vm) { };
public:
    // runFrame: when it gets OpCall, it sets up the frame and return to the containing VM
    // VM will then run the topmost frame on callFrames,
    // runFrame also returns to containing VM when it gets OpReturn
    void run();
    Chunk& getCurrentChunk();
    uint8_t readByte();
    OpCode readOpCode();
    Value readConstant();
    int getCurrentLine();
    void pushStack(Value value);
    Value popStack();
    Value peekStack(int relativeIndex);
};


#endif //LOX_INTERPRETER_CALLFRAME_H
