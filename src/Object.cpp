
#include "Object.h"

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