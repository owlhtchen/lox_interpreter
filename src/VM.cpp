
#include <VM.h>
#include <DerivedObject.h>


int VM::createCallFrame(ClosureObj* callableObj, int stackBase) {
    callFrames.push_back(CallFrame(callableObj, stackBase, *this));
    return callFrames.size() - 1;
}

void VM::setUpFunctionCall(ClosureObj *callableObj, int argCount) {
    createCallFrame(callableObj, stack.size() - 1 - argCount);
}


void VM::run() {
    while (!callFrames.empty()) {
        // runFrame returns when it needs to start a new function call (needs a new CallFrame)
        // i.e. it gets OP_RETURN or OP_CALL
        callFrames.back().runFrame();
    }
}

void VM::start(FunctionObj *functionObj) {
    auto closureObj = new ClosureObj(functionObj);
    stack.emplace_back(closureObj);
    setUpFunctionCall(closureObj, 0);
    run();
}
