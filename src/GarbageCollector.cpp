
#include <GarbageCollector.h>
#include <Object.h>

GarbageCollector &GarbageCollector::getInstance() {
    // the only instance
    static GarbageCollector gc;
    return gc;
}

