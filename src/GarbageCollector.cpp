
#include <GarbageCollector.h>
#include <DerivedObject.h>

GarbageCollector &GarbageCollector::getInstance() {
    // the only instance
    static GarbageCollector gc;
    return gc;
}

void GarbageCollector::freeAllObjects() {
    Object *current, *prev;
    current = allObjects;
    while(current != nullptr) {
        prev = current;
        current = current->next;
        free(prev);
    }
}

UpValueObj *GarbageCollector::addUpValue(Value *location) {
    auto newUpValue = new UpValueObj(location);
    if(allOpenUpValues == nullptr) {
        allOpenUpValues = newUpValue;
        return newUpValue;
    } else {
        UpValueObj *current, *prev;
        prev = nullptr;
        // current <- newUpValue <- prev
        for(current = allOpenUpValues; current != nullptr && location < current->location; ) {
            prev = current;
            current = current->next;
        }
        if(current != nullptr && current->location == location) {
            // this stack value is already  captured before
            return current;
        }
        newUpValue->next = current;
        if(prev == nullptr) {
            allOpenUpValues = newUpValue;
        } else {
            prev->next = newUpValue;
        }
        return newUpValue;
    }
}

