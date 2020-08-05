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

void StringPool::deleteString() {
    for(auto it = pool.begin(), next_it = it; it != pool.cend(); it = next_it) {
        ++next_it;
        if(!it->second->isMarked) {
            pool.erase(it);
        }
    }
}

std::string StringObj::toString() {
    return str;
}

void StringObj::mark() {
    Object::mark();
}

std::string FunctionObj::toString() {
    return "function " + name;
}

void FunctionObj::mark() {
    Object::mark();
    chunk.markConstants();
}

FunctionObj::~FunctionObj() {
    chunk.freeConstantObjs();
}


std::string UpValueObj::toString() {
    return "UpValue: " + ::toString(*location);
}

void UpValueObj::mark() {
    Object::mark();
    markValue(&closed);
}

UpValueObj::~UpValueObj() {
    freeValue(&closed);
}

std::string ClosureObj::toString() {
    return "closure: " + functionObj->getName();
}

ClosureObj::ClosureObj(FunctionObj *functionObj):
        functionObj(functionObj), closureCount(functionObj->closureCount) {
}

void ClosureObj::mark() {
    Object::mark();
    functionObj->mark();
    for(const auto & upValueObj: upValues) {
        upValueObj->mark();
    }
}

ClosureObj::~ClosureObj() {
    free(functionObj);
    for(auto upValue: upValues) {
        free(upValue);
    }
}


std::string ClassObj::toString() {
    return "Class: " + name->toString();
}

void ClassObj::mark() {
    Object::mark();
    name->mark();
    for(auto& it: methods) {
        it.first->mark();
        it.second->mark();
    }
}

ClassObj::~ClassObj() {
    for(auto &it: methods) {
        free(it.second);
    }
}

std::string InstanceObj::toString() {
    std::string str = "Instance: of " + klass->toString();
    if(!fields.empty()) {
        str += " with field(s): ";
        for(auto const & pair: fields) {
            str += "{" + pair.first->toString() + ": " + ::toString(pair.second) + "} ";
        }
    }
    return str;
}

void InstanceObj::mark() {
    Object::mark();
    klass->mark();
    for(auto& it: fields) {
        it.first->mark();
        markValue(&it.second);
    }
}

InstanceObj::~InstanceObj() {
    free(klass);
    for(auto& it: fields) {
        freeValue(&it.second);
    }
}

std::string ClassMethodObj::toString() {
    return "method (" + method->toString() + ") of instance " + "(" + receiver->toString() + ")";
}

void ClassMethodObj::mark() {
    Object::mark();
    receiver->mark();
    method->mark();
}

ClassMethodObj::~ClassMethodObj() {
    free(receiver);
    free(method);
}
