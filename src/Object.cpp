#include <Object.h>
#include <GarbageCollector.h>
#include <variant>

Object::Object(): isMarked(true), next(nullptr) {
    // TODO: call gc if necessary

    isMarked = false;
}

//template<typename T>
//bool Object::isa() {
//        return typeid(*this) == typeid(T);
//    }
//
//template<typename T>
//T * Object::cast() {
//    if(this->isa<T>()) {
//        return static_cast<T*>(this);
//    }
//}
//
//template<typename T>
//T * Object::dyn_cast() {
//    return dynamic_cast<T*>(this);
//}


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