#pragma once

#include <type_traits>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <array>
#include <optional>
#include <memory>
#include <forward_list>
#include <variant>
#include <string>
#include <tuple>

namespace otas_serializer {

struct any_type {
    template <class T>
    operator T() const;
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

template <class T>
struct fake_obj {
    inline static T value{};
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
    inline constexpr static auto static_tuple_view() { \
        auto &[__VA_ARGS__] = fake_obj<T>::value; \
        auto refs = std::tie(__VA_ARGS__); \
        auto function = [](auto &...ref) { \
            return std::make_tuple(&ref...); \
        }; \
        return std::apply(function, refs); \
    } \
}

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
GENERATE_TEMPLATE(17, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16);
GENERATE_TEMPLATE(18, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17);
GENERATE_TEMPLATE(19, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18);
GENERATE_TEMPLATE(20, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19);
GENERATE_TEMPLATE(21, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20);
GENERATE_TEMPLATE(22, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21);
GENERATE_TEMPLATE(23, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22);
GENERATE_TEMPLATE(24, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23);
GENERATE_TEMPLATE(25, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24);
GENERATE_TEMPLATE(26, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25);
GENERATE_TEMPLATE(27, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26);
GENERATE_TEMPLATE(28, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27);
GENERATE_TEMPLATE(29, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28);
GENERATE_TEMPLATE(30, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29);
GENERATE_TEMPLATE(31, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30);
GENERATE_TEMPLATE(32, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, 
    f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31);

#undef GENERATE_TEMPLATE

template <class T>
inline constexpr std::string_view type_name() {
    constexpr std::string_view function_name = __PRETTY_FUNCTION__;
    auto l = function_name.find("= ");
    auto r = function_name.find(";");
    return function_name.substr(l + 2, r - l - 2);
}


template <auto T>
inline constexpr std::string_view member_name() {
    constexpr std::string_view function_name = __PRETTY_FUNCTION__;
    auto r = function_name.find(");");
    auto l = function_name.substr(0, r).rfind("::");
    return function_name.substr(l + 2, r - l - 2);
}

template <class T>
struct member_name_helper {
    inline constexpr static auto tuple_name() {
        constexpr auto count = get_member_count<T>();
        constexpr auto members = member_tuple_helper<T, count>::static_tuple_view();
        std::array<std::string_view, count> arr;
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((arr[index] = member_name<std::get<index>(members)>()), ...);
        } (std::make_index_sequence<count>{});
        return arr;
    }
};

template <class T>
concept map_container_emplace = requires(T container) {
    container.emplace(typename T::key_type{}, typename T::mapped_type{});
};

template <class T>
concept map_container = requires(T container) {
    typename T::key_type;
    typename T::mapped_type;
    container.size();
    container.begin();
    container.end();
    container.insert({typename T::key_type{}, typename T::mapped_type{}});
};

template <class T>
concept normal_container_emplace = requires(T container) {
    container.emplace(typename T::value_type{});
};

template <class T>
concept set_container = requires(T container) {
    typename T::key_type;
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
    container.insert(typename T::value_type{});
};

template <class T>
concept vector_container = requires(T container, unsigned int n) {
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
    container.resize(n);
    { container[n] } -> std::same_as<typename T::value_type &>;
};

template <class T>
concept list_container = requires(T container) {
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
    container.insert(typename T::value_type{});
};

template <class T>
concept char_container = std::is_same_v<T, char> || std::is_same_v<T, signed char>
    || std::is_same_v<T, unsigned char> || std::is_same_v<T, wchar_t>
    || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t>
    || std::is_same_v<T, char32_t>;

template <class T>
concept string_container = requires(T container, unsigned int n) {
    requires char_container<typename T::value_type>;
    container.size();
    container.begin();
    container.end();
    container.length();
    container.data();
    container.resize(n);
    { container[n] } -> std::same_as<typename T::value_type &>;
};

template <class T>
concept normal_container = requires(T container) {
    typename T::value_type;
};



}