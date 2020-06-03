
#ifndef LOX_INTERPRETER_VM_H
#define LOX_INTERPRETER_VM_H

#include <vector>
#include <cstdint>
#include <CallFrame.h>

class Object;
class CallableObj;

class VM {
    friend class CallFrame;
private:
//    Chunk chunk;
//    int ip;
    std::vector<CallFrame> callFrames;
    std::vector<Value> stack;
public:
    // should take in a functionObj
    explicit VM() = default;
//    void setUpFunctionCall(CallableObj* callableObj, int argCount);
};


#endif //LOX_INTERPRETER_VM_H
