
#ifndef LOX_INTERPRETER_VALUE_H
#define LOX_INTERPRETER_VALUE_H

#include <variant>
#include <string>

class Object;

using Value = std::variant<std::monostate, double, bool, Object*>;

std::string toString(Value value);  // for display

//template <typename T>
//T* castToObj(Value* value) {
//    auto tmp = std::get_if<Object*>(value);
//    if(tmp == nullptr) {
//        return nullptr;
//    }
//    return (*tmp)->dyn_cast<T>();
//}

#endif //LOX_INTERPRETER_VALUE_H
