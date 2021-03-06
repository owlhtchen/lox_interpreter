
#include <VM.h>
#include <DerivedObject.h>
#include <iostream>
#include <GarbageCollector.h>
#include <GarbageCollector.h>
#include <debug.h>

VM::VM() {
    GarbageCollector::getInstance().setVM(this);
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
    auto closureObj = GarbageCollector::getInstance().addObject(new ClosureObj(functionObj));
    stack.push_back(closureObj);
    createCallFrame(closureObj, 0);
    run();
}

void VM::markStack() {
#ifdef GC_DEBUG_PRINT
    std::cout << "markStack\n";
#endif
    for(int i = 0; i < stack.size(); i++) {
        markValue(&stack[i]);
    }
}

void VM::markCallFrames() {
#ifdef GC_DEBUG_PRINT
    std::cout << "markCallFrames\n";
#endif
    for(auto& callFrame: callFrames) {
        callFrame.markCallFrame();
    }
}

void VM::markGlobals() {
#ifdef GC_DEBUG_PRINT
    std::cout << "markGlobals\n";
#endif
    for(auto& it: globals) {
        it.first->mark();
        markValue(&it.second);
    }
}