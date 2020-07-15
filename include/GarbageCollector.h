
#ifndef LOX_INTERPRETER_GARBAGECOLLECTOR_H
#define LOX_INTERPRETER_GARBAGECOLLECTOR_H
// https://stackoverflow.com/questions/86582/singleton-how-should-it-be-used

#include <DerivedObject.h>

class GarbageCollector {
private:
    Object* allObjects;
    // private constructor
    GarbageCollector(): allObjects(nullptr) {};
public:
    UpValueObj* allOpenUpValues;  // sorted by UpValueObj.location
    // prevent compiler from generating copy & assignment methods
    GarbageCollector(GarbageCollector const& copy) = delete;
    GarbageCollector& operator= (GarbageCollector const& copy) = delete;
    static GarbageCollector& getInstance();
    template <typename T>
    T* addObject(T* _object);
    void freeAllObjects();
    UpValueObj* addUpValue(Value* location);
};

template <typename T>
T* GarbageCollector::addObject(T * _object) {
    auto* object = (Object* ) _object;
    auto& gc = GarbageCollector::getInstance();
    object->next = gc.allObjects;
    gc.allObjects = object;
    return _object;
}
#endif //LOX_INTERPRETER_GARBAGECOLLECTOR_H
