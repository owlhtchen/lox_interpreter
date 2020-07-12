
#ifndef LOX_INTERPRETER_UPVALUE_H
#define LOX_INTERPRETER_UPVALUE_H


class UpValue { // emulate UpValueObj when compiling
public:
    int index;
    bool isLocal;
    UpValue(int upValueIndex, bool isLocal):
    index(upValueIndex), isLocal(isLocal) { };
};


#endif //LOX_INTERPRETER_UPVALUE_H
