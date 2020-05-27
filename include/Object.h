
#ifndef LOX_INTERPRETER_OBJECT_H
#define LOX_INTERPRETER_OBJECT_H

class Object {
private:
    Object* next;
    bool isMarked;
    // isMarked should be set to true during initialization to prevent the GC from cleaning
    // the newly created object, and set back to false to prepare for next 'mark-sweep'
};

#endif //LOX_INTERPRETER_OBJECT_H
