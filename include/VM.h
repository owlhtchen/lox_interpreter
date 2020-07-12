
#ifndef LOX_INTERPRETER_VM_H
#define LOX_INTERPRETER_VM_H

#include <vector>
#include <cstdint>
#include <CallFrame.h>
#include "DerivedObject.h"
#include <unordered_map>

class Object;
class FunctionObj;

class VM {
    friend class CallFrame;
private:
//    Chunk chunk;
//    int ip;
    std::vector<CallFrame> callFrames;
    std::vector<Value> stack;
    std::unordered_map<StringObj*, Value> globals;
public:
    // should take in a functionObj
    explicit VM() = default;
    int createCallFrame(ClosureObj* callableObj, int stackBase);
    void setUpFunctionCall(ClosureObj* callableObj, int argCount);
    void start(FunctionObj* functionObj);
    void run();
};


#endif //LOX_INTERPRETER_VM_H
