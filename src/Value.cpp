#include <Value.h>
#include <Object.h>
#include <type_traits>

template<typename> inline constexpr bool always_false_v = false;

std::string toString(Value value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return "Nil";
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, Object*>) {
            return arg->toString();
        } else {
            static_assert(always_false_v<T>, "unexpected type in Value.toString");
            return "";
        }
    }, value);
}
