
#include <VM.h>
#include <DerivedObject.h>
#include <iostream>
#include <debug.h>

VM::VM() {
#ifdef VECTOR
    stack.reserve(10000);
#endif
}

int VM::createCallFrame(ClosureObj* callableObj, int argCount) {
    int stackBase = stack.size() - 1 - argCount;
    callFrames.push_back(CallFrame(callableObj, stackBase, *this));
    return callFrames.size() - 1;
}

void VM::run() {
    while (!callFrames.empty()) {
        // runFrame returns when it needs to start a new function call (needs a new CallFrame)
        // i.e. it gets OP_RETURN or OP_CALL
        callFrames.back().runFrame();
    }
}

void VM::start(FunctionObj *functionObj) {
    std::cout << " --- vm started ---" << std::endl;
    auto closureObj = new ClosureObj(functionObj);
    stack.push_back(closureObj);
    createCallFrame(closureObj, 0);
    run();
}
