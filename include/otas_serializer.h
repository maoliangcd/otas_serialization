#include <array>
#include <string>

namespace otas_serializer {

struct any_type {
    template <typename T>
    operator T();
};
template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T, typename = void, typename ...Args>
struct member_count_struct {
    constexpr static size_t value = sizeof...(Args) - 1;
};
template <typename T, typename ...Args>
struct member_count_struct<T, std::void_t<decltype(T{Args{}...})>, Args...> {
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
};

#define GENERATE_TEMPLATE(n, ...) \
template <class T> \
struct member_tuple_helper<T, n> { \
    inline constexpr static auto tuple_view(T &t) { \
        auto &&[__VA_ARGS__] = t; \
        return std::tie(__VA_ARGS__); \
    } \
} \

GENERATE_TEMPLATE(1, f0);
GENERATE_TEMPLATE(2, f0, f1);

#define serialize(t, s) \
[](auto &&t, auto &&s) -> auto { \
    std::size_t offset{}; \
    serialize_template<remove_cvref_t<decltype(t)>>(t, s, offset); \
}(t, s) \

template <class T>
auto serialize_template(T &t, std::string &s, std::size_t &offset) {
    constexpr auto count = get_member_count<T>();
    auto members = member_tuple_helper<T, count>::tuple_view(t);
    [&]<std::size_t... index>(std::index_sequence<index...>) {
        ((serialize_template<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>(std::get<index>(members), s, offset)), ...);
    } (std::make_index_sequence<count>{});
    return ;
}

#define deserialize(s, t) \
[](auto &&s, auto &&t) -> auto {\
    std::size_t offset{}; \
    deserialize_template<remove_cvref_t<decltype(t)>>(s, t, offset); \
}(s, t) \

template <class T>
auto deserialize_template(std::string &s, T &t, std::size_t &offset) {
    constexpr auto count = get_member_count<T>();
    auto members = member_tuple_helper<T, count>::tuple_view(t);
    [&]<std::size_t... index>(std::index_sequence<index...>) {
        ((deserialize_template<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>(s, std::get<index>(members), offset)), ...);
    } (std::make_index_sequence<count>{});
    return ;
}

#define GENERATE_TEMPLATE_BASIC_TYPE(type) \
template<> \
auto serialize_template<type>(type &t, std::string &s, std::size_t &offset) { \
    s.append(reinterpret_cast<char *>(&t), sizeof(t)); \
    offset += sizeof(t); \
    return ; \
}; \
template <> \
auto deserialize_template<type>(std::string &s, type &t, std::size_t &offset) { \
    memcpy(&t, &s[offset], sizeof(t)); \
    offset += sizeof(type); \
    return ; \
} \

GENERATE_TEMPLATE_BASIC_TYPE(int);
GENERATE_TEMPLATE_BASIC_TYPE(unsigned int);
GENERATE_TEMPLATE_BASIC_TYPE(long);
GENERATE_TEMPLATE_BASIC_TYPE(unsigned long);
GENERATE_TEMPLATE_BASIC_TYPE(long long);
GENERATE_TEMPLATE_BASIC_TYPE(unsigned long long);
GENERATE_TEMPLATE_BASIC_TYPE(float);
GENERATE_TEMPLATE_BASIC_TYPE(double);
GENERATE_TEMPLATE_BASIC_TYPE(char);

}