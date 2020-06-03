
#include <VM.h>
#include <Object.h>


int VM::createCallFrame(FunctionObj* callableObj, int stackBase) {
    callFrames.emplace_back(callableObj, stackBase, *this);
    return callFrames.size() - 1;
}

void VM::setUpFunctionCall(FunctionObj *functionObj, int argCount) {
    auto closure = functionObj;
    createCallFrame(closure, stack.size() - 1 - argCount);
}


void VM::run() {
    while (!callFrames.empty()) {
        callFrames.back().runFrame();
    }
}

void VM::start(FunctionObj *functionObj) {
    stack.emplace_back(functionObj);
    setUpFunctionCall(functionObj, 0);
    run();
}
