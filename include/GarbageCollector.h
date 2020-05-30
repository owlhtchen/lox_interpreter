
#ifndef LOX_INTERPRETER_GARBAGECOLLECTOR_H
#define LOX_INTERPRETER_GARBAGECOLLECTOR_H
// https://stackoverflow.com/questions/86582/singleton-how-should-it-be-used

#include <Object.h>

class GarbageCollector {
private:
    Object* allObjects;
    // private constructor
    GarbageCollector(): allObjects(nullptr) {};
public:
    // prevent compiler from generating copy & assignment methods
    GarbageCollector(GarbageCollector const& copy) = delete;
    GarbageCollector& operator= (GarbageCollector const& copy) = delete;
    static GarbageCollector& getInstance();
    template <typename T>
    T* addObject(T* _object);
    void freeAllObjects();
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
