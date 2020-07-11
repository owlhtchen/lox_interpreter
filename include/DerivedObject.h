
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
    int getArity() { return arity; }
    std::string getName() { return name; }
};

#endif //LOX_INTERPRETER_DERIVEDOBJECT_H
