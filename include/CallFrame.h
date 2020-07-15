
#ifndef LOX_INTERPRETER_CALLFRAME_H
#define LOX_INTERPRETER_CALLFRAME_H

#include <Chunk.h>
#include <Value.h>
#include <OpCode.h>
#include <vector>
#include "VM.h"

class ClosureObj;
class UpValueObj;
class VM;

class CallFrame {
    friend class VM;
private:
    ClosureObj* closureObj;
    int ip;
    int stackBase;
    //vm: a reference to the vm which contains this CallFrame
    VM & vm;
    // CallFrame should only be constructed with the containing VM
    CallFrame(ClosureObj* closureObj, int stackBase, VM& vm):
            closureObj(closureObj), stackBase(stackBase), ip(0), vm(vm) { };
public:
    // runFrame: when it gets OpCall, it sets up the frame and return to the containing VM
    // VM will then run the topmost frame on callFrames,
    // when VM gets OpReturn, runFrame pops the current callFrame returns to containing VM
    void runFrame();
    Chunk& getCurrentChunk();
    uint8_t readByte();
    OpCode readOpCode();
    Value readConstant();
    int getCurrentLine();
    void pushStack(Value value);
    Value popStack();
    Value peekStackBase(int relativeIndex);
    void setStackBase(int relativeIndex, const Value & newValue);
    Value peekStackTop(int relativeIndex = 0);
    UpValueObj* captureUpValue(int localVarIndex);
    void closeUpValue();
    void closeUpValues(Value* location);
    void opCallValue(Value callee, int actualArity);
};


#endif //LOX_INTERPRETER_CALLFRAME_H
