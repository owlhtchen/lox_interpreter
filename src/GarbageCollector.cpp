
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

