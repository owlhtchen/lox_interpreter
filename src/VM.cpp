
#include <VM.h>
#include <DerivedObject.h>


int VM::createCallFrame(FunctionObj* callableObj, int stackBase) {
    callFrames.push_back(CallFrame(callableObj, stackBase, *this));
    return callFrames.size() - 1;
}

void VM::setUpFunctionCall(FunctionObj *functionObj, int argCount) {
    auto closure = functionObj;
    createCallFrame(closure, stack.size() - 1 - argCount);
}


void VM::run() {
    while (!callFrames.empty()) {
        // runFrame returns when it needs to start a new function call (needs a new CallFrame)
        // i.e. it gets OP_RETURN or OP_CALL
        callFrames.back().runFrame();
    }
}

void VM::start(FunctionObj *functionObj) {
    stack.emplace_back(functionObj);
    setUpFunctionCall(functionObj, 0);
    run();
}
