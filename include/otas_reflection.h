#pragma once

#include <type_traits>

namespace otas_serializer {

struct any_type {
    template <class T>
    operator T();
};
template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T, class = void, class ...Args>
struct member_count_struct {
    constexpr static size_t value = sizeof...(Args) - 1;
};
template <class T, class ...Args>
struct member_count_struct<T, std::void_t<decltype(T{{Args{}}...})>, Args...> {
    constexpr static size_t value = member_count_struct<T, void, Args..., any_type>::value;
};

template <class T>
inline constexpr std::size_t get_member_count() {
    return member_count_struct<remove_cvref_t<T>>::value;
}

template <class T, std::size_t n>
struct member_tuple_helper {
    inline constexpr static auto tuple_view(T &t) {
        return std::tie();
    }
    inline constexpr static auto tuple_view(const T &t) {
        return std::tie();
    }
};

#define GENERATE_TEMPLATE(n, ...) \
template <class T> \
struct member_tuple_helper<T, n> { \
    inline constexpr static auto tuple_view(T &t) { \
        auto &&[__VA_ARGS__] = t; \
        return std::tie(__VA_ARGS__); \
    } \
    inline constexpr static auto tuple_view(const T &t) { \
        auto &&[__VA_ARGS__] = t; \
        return std::tie(__VA_ARGS__); \
    } \
} \

GENERATE_TEMPLATE(1, f0);
GENERATE_TEMPLATE(2, f0, f1);
GENERATE_TEMPLATE(3, f0, f1, f2);
GENERATE_TEMPLATE(4, f0, f1, f2, f3);
GENERATE_TEMPLATE(5, f0, f1, f2, f3, f4);
GENERATE_TEMPLATE(6, f0, f1, f2, f3, f4, f5);
GENERATE_TEMPLATE(7, f0, f1, f2, f3, f4, f5, f6);
GENERATE_TEMPLATE(8, f0, f1, f2, f3, f4, f5, f6, f7);
GENERATE_TEMPLATE(9, f0, f1, f2, f3, f4, f5, f6, f7, f8);
GENERATE_TEMPLATE(10, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9);
GENERATE_TEMPLATE(11, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10);
GENERATE_TEMPLATE(12, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11);
GENERATE_TEMPLATE(13, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12);
GENERATE_TEMPLATE(14, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13);
GENERATE_TEMPLATE(15, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14);
GENERATE_TEMPLATE(16, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15);

#undef GENERATE_TEMPLATE

}