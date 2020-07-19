
#ifndef LOX_INTERPRETER_VM_H
#define LOX_INTERPRETER_VM_H

#include <vector>
#include <cstdint>
#include <CallFrame.h>
#include "DerivedObject.h"
#include <unordered_map>
#include <Stack.h>
#include <debug.h>

class Object;
class FunctionObj;
class GarbageCollector;

class VM {
    friend class CallFrame;
    friend class GarbageCollector;
private:
//    Chunk chunk;
//    int ip;
    std::vector<CallFrame> callFrames;
    // stack should not be resized because upValue.position needs to refer to a fixed address.
#ifdef VECTOR
    std::vector<Value> stack;
#else
    Stack<Value, 16384> stack;
#endif
    std::unordered_map<StringObj*, Value> globals;
public:
    // should take in a functionObj
    VM();
    int createCallFrame(ClosureObj* callableObj, int argCount);
    void start(FunctionObj* functionObj);
    void run();
};


#endif //LOX_INTERPRETER_VM_H
