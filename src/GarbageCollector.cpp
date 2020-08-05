
#include <GarbageCollector.h>
#include <DerivedObject.h>
#include <iostream>
#include <VM.h>

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
        return GarbageCollector::getInstance().addObject(newUpValue);
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
        return GarbageCollector::getInstance().addObject(newUpValue);
    }
}

void GarbageCollector::setVM(VM *_vm) {
    vm = _vm;
}

void GarbageCollector::printAllObjects() {
    auto temp = allObjects;
    while (temp != nullptr) {
        std::cout << temp->toString() << ": " << temp->isMarked << "; ";
        temp = temp->next;
    }
    std::cout << std::endl;
}

void GarbageCollector::sweepObjects() {
    if(!vm) {
        return;
    }
    auto current = allObjects;
    Object* prev = nullptr;
    while (current != nullptr) {
        if(!current->isMarked) {
#ifdef GC_DEBUG_PRINT
            std::cout << "- deleted: " << current->toString() << "; ";
#endif
            auto temp = current;
            if(prev) {
                prev->next = current->next;
            } else {
                allObjects = current->next;
            }
            current = current->next;
            free(temp);
        } else {
            current->isMarked = false;
            prev = current;
            current = current->next;
        }
    }
#ifdef GC_DEBUG_PRINT
    std::cout << std::endl;
#endif
}

void GarbageCollector::markObjects() {
    if(!vm) {
        return;
    }
    vm->markStack();
    vm->markCallFrames();
    vm->markGlobals();
    markOpenUpValues();
}

void GarbageCollector::markSweep() {
    markObjects();
    StringPool::getInstance().deleteString();
    sweepObjects();
}

void GarbageCollector::markOpenUpValues() {
    auto temp = allOpenUpValues;
    while (temp != nullptr) {
        temp->mark();
        temp = temp->next;
    }
}
