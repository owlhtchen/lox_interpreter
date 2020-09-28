
#include "Object.h"
#include <GarbageCollector.h>
#include <iostream>
#include <debug.h>

Object::Object(): isMarked(true), next(nullptr) {
    // https://www.learncpp.com/cpp-tutorial/114-constructors-and-initialization-of-derived-classes/
    // TODO: call gc if necessary (decided to call it in GarbageCollector::addObject after the object is fully constructed so I can call object.mark() )

//    GarbageCollector::getInstance().markSweep();

    isMarked = false;
}

void Object::mark() {
    isMarked = true;
#ifdef GC_DEBUG_PRINT
    std::cout << "marked: " << this->toString() << "; " << std::endl;
#endif
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