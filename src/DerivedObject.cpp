#include <DerivedObject.h>
#include <GarbageCollector.h>
#include <variant>
#include <Value.h>


StringPool &StringPool::getInstance() {
    static StringPool stringPool;
    return stringPool;
}

StringObj *StringPool::getStringObj(const std::string& str) {
    auto& temp = StringPool::getInstance().pool;
    auto ptr = temp.find(str);
    if(ptr == temp.end()) {
        auto str_ptr = new StringObj(str);
        pool[str] = GarbageCollector::getInstance().addObject(str_ptr);
        return str_ptr;
    } else {
        return ptr->second;
    }
}

std::string StringObj::toString() {
    return str;
}

std::string FunctionObj::toString() {
    return "function " + name;
}

std::string ClosureObj::toString() {
    return "closure: " + functionObj->getName();
}

ClosureObj::ClosureObj(FunctionObj *functionObj):
    functionObj(functionObj), closureCount(functionObj->closureCount) {
}

std::string UpValueObj::toString() {
    return "UpValue: " + ::toString(*location);
}
