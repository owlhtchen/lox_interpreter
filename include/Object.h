
#ifndef LOX_INTERPRETER_OBJECT_H
#define LOX_INTERPRETER_OBJECT_H

#include <typeinfo>
#include <Error.h>
#include <string>
#include <unordered_map>
#include <Chunk.h>
#include <Value.h>
#include <string>
#include <stdexcept>

class GarbageCollector;
class CodeGenerator;
class CallFrame;

class Object {
    friend class GarbageCollector;
private:
    Object* next;
    bool isMarked;
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
};

class StringObj: public Object {
    friend class VM;
    friend class CallFrame;
private:
    std::string str;
public:
    explicit StringObj(std::string str): str(std::move(str)) {};
    std::string toString() override ;
};

class StringPool {
private:
    std::unordered_map<std::string, StringObj*> pool;
    StringPool() = default;
public:
    StringPool(StringPool const& copy) = delete;
    StringPool& operator= (StringPool const & copy) = delete;
    static StringPool& getInstance();
    StringObj* getStringObj(const std::string& str);  // non-static
};

class FunctionObj;

//class CallableObj {
//public:
//    virtual Value getThis() = 0;
//    virtual FunctionObj* getClosure() = 0;
//};

class FunctionObj: public Object {
    friend class GarbageCollector;
    friend class CodeGenerator;
    friend class VM;
private:
    std::string name;
    int arity;
    Chunk chunk;
public:
    explicit FunctionObj(std::string name = ""): name(std::move(name)), arity(0){ };
//    FunctionObj(): name(""), arity(0) { };
    Chunk& getChunk() { return chunk; };
    std::string toString() override ;
};

#endif //LOX_INTERPRETER_OBJECT_H
