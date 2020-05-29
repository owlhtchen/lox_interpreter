
#ifndef LOX_INTERPRETER_OBJECT_H
#define LOX_INTERPRETER_OBJECT_H

#include <typeinfo>
#include <Error.h>
#include <string>
#include <unordered_map>

class GarbageCollector;

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
    bool isa();
    template <typename T>
    T* cast();
    template <typename T>
    T* dyn_cast();
};

class StringObj: public Object {
private:
    std::string str;
public:
    explicit StringObj(std::string str): str(std::move(str)) {};
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

#endif //LOX_INTERPRETER_OBJECT_H
