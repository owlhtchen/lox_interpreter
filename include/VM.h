
#ifndef LOX_INTERPRETER_VM_H
#define LOX_INTERPRETER_VM_H

#include <vector>
#include <cstdint>
#include <CallFrame.h>
#include "Object.h"


class Object;
class CallableObj;
class FunctionObj;

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
    int createCallFrame(FunctionObj* callableObj, int stackBase);
    void setUpFunctionCall(FunctionObj* callableObj, int argCount);
    void start(FunctionObj* functionObj);
    void run();
};


#endif //LOX_INTERPRETER_VM_H
