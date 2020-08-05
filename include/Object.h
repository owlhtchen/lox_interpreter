
#ifndef LOX_INTERPRETER_OBJECT_H
#define LOX_INTERPRETER_OBJECT_H

#include <typeinfo>
#include <string>
#include <Error.h>
#include <stdexcept>

class GarbageCollector;
class CodeGenerator;
class CallFrame;

class Object {
    friend class GarbageCollector;
private:
    Object* next;
    // isMarked should be set to true during initialization to prevent the GC from cleaning
    // the newly created object, and set back to false (after gc has done its work)
    // to prepare for next 'mark-sweep'
public:
    Object();
    virtual ~Object() = default;
    // llvm style rtti
    template <typename T>
    bool isa() {
        return typeid(*this) == typeid(T);
    }
    template <typename T>
    T* cast() {
        if(this->isa<T>()) {
            return static_cast<T*>(this);
        }
        throw std::logic_error("cast to wrong type in Object.cast");
    }
    template <typename T>
    T* dyn_cast() {
        return dynamic_cast<T*>(this);
    }
    virtual std::string toString() = 0;
    virtual void mark();

    bool isMarked;
};

#endif //LOX_INTERPRETER_OBJECT_H
