
#ifndef LOX_INTERPRETER_DERIVEDOBJECT_H
#define LOX_INTERPRETER_DERIVEDOBJECT_H

#include <typeinfo>
#include <Error.h>
#include <unordered_map>
#include <Chunk.h>
#include <Value.h>
#include <string>
#include <stdexcept>
#include <Object.h>

class GarbageCollector;
class CodeGenerator;
class CallFrame;


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

class FunctionObj: public Object {
    friend class GarbageCollector;
    friend class CodeGenerator;
    friend class VM;
private:
    std::string name;
    int arity;
    Chunk chunk;
public:
    int closureCount;
    explicit FunctionObj(std::string name = ""): name(std::move(name)), arity(0){ };
//    FunctionObj(): name(""), arity(0) { };
    Chunk& getChunk() { return chunk; };
    std::string toString() override ;
    int getArity() { return arity; }
    std::string getName() { return name; }
};

class UpValueObj: public Object {
public:
    Value* location;
    Value closed;
    UpValueObj* next;
    UpValueObj(Value * location):
        location(location), closed(std::monostate()), next(nullptr) { };
    std::string toString() override ;
};

class ClosureObj: public Object {
public:
    int closureCount;
    FunctionObj* functionObj;
    std::vector<UpValueObj*> upValues;
    explicit ClosureObj(FunctionObj* functionObj);
    std::string toString() override ;
};

class ClassObj: public Object {
public:
    StringObj* name;
    std::unordered_map<StringObj*, ClosureObj*> methods;
    explicit ClassObj(StringObj* name): name(name) { };
    std::string toString() override;
};

class InstanceObj: public Object {
public:
    ClassObj* klass;
    std::unordered_map<StringObj*, Value> fields;
    explicit InstanceObj(ClassObj* klass):
        klass(klass) { };
    std::string toString() override ;
};

class ClassMethodObj: public Object {
public:
    InstanceObj* receiver;
    ClosureObj* method;
    ClassMethodObj(InstanceObj* receiver, ClosureObj*  method):
        receiver(receiver), method(method) { };
    std::string toString() override ;
};

#endif //LOX_INTERPRETER_DERIVEDOBJECT_H
