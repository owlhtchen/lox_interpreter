
#ifndef LOX_INTERPRETER_VALUE_H
#define LOX_INTERPRETER_VALUE_H

#include <variant>
class Object;

using Value = std::variant<std::monostate, double, bool, Object*>;

#endif //LOX_INTERPRETER_VALUE_H
