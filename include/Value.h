
#ifndef LOX_INTERPRETER_VALUE_H
#define LOX_INTERPRETER_VALUE_H

#include <variant>
#include <string>

class Object;

using Value = std::variant<std::monostate, double, bool, Object*>;

std::string toString(Value value);

#endif //LOX_INTERPRETER_VALUE_H
