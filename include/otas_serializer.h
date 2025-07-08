#include <array>
#include <string>
#include <tuple>
#include <vector>
#include <map>

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
GENERATE_TEMPLATE(3, f0, f1, f2);
GENERATE_TEMPLATE(4, f0, f1, f2, f3);
GENERATE_TEMPLATE(5, f0, f1, f2, f3, f4);
GENERATE_TEMPLATE(6, f0, f1, f2, f3, f4, f5);
GENERATE_TEMPLATE(7, f0, f1, f2, f3, f4, f5, f6);
GENERATE_TEMPLATE(8, f0, f1, f2, f3, f4, f5, f6, f7);

#define serialize(t, s) \
[](auto &&t, auto &&s) -> auto { \
    std::size_t offset{}; \
    serialize_helper<remove_cvref_t<decltype(t)>>::serialize_template(t, s, offset); \
}(t, s) \

template <class T>
struct serialize_helper {
    static auto serialize_template(T &t, std::string &s, std::size_t &offset) {
        constexpr auto count = get_member_count<T>();
        auto members = member_tuple_helper<T, count>::tuple_view(t);
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), s, offset)), ...);
        } (std::make_index_sequence<count>{});
        return ;
    }
};

#define deserialize(s, t) \
[](auto &&s, auto &&t) -> auto {\
    std::size_t offset{}; \
    deserialize_helper<remove_cvref_t<decltype(t)>>::deserialize_template(s, t, offset); \
}(s, t) \

template <class T>
struct deserialize_helper {
    static auto deserialize_template(std::string &s, T &t, std::size_t &offset) {
        constexpr auto count = get_member_count<T>();
        auto members = member_tuple_helper<T, count>::tuple_view(t);
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((deserialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::deserialize_template(s, std::get<index>(members), offset)), ...);
        } (std::make_index_sequence<count>{});
        return ;
    }
};

#define GENERATE_TEMPLATE_BASIC_TYPE(type) \
template<> \
struct serialize_helper<type> { \
    static auto serialize_template(type &t, std::string &s, std::size_t &offset) { \
        s.append(reinterpret_cast<char *>(&t), sizeof(t)); \
        offset += sizeof(t); \
        return ; \
    } \
}; \
template <> \
struct deserialize_helper<type> { \
    static auto deserialize_template(std::string &s, type &t, std::size_t &offset) { \
        memcpy(&t, &s[offset], sizeof(t)); \
        offset += sizeof(type); \
        return ; \
    } \
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

template <>
struct serialize_helper<std::string> {
    static auto serialize_template(std::string &t, std::string &s, std::size_t &offset) {
        std::size_t size = t.length();
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        s += t;
        offset += size;
        return ;
    }
};
template <>
struct deserialize_helper<std::string> {
    static auto deserialize_template(std::string &s, std::string &t, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        t = std::string(&s[offset], size);
        offset += size;
        return ;
    }
};


template <class T>
struct serialize_helper<std::vector<T>> {
    static auto serialize_template(std::vector<T> &vec, std::string &s, std::size_t &offset) {
        std::size_t size = vec.size();
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        for (auto &item : vec) {
            serialize_helper<T>::serialize_template(item, s, offset);
        }
        return ;
    }
};
template <class T>
struct deserialize_helper<std::vector<T>> {
    static auto deserialize_template(std::string &s, std::vector<T> &vec, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        vec.resize(size);
        for (auto &item : vec) {
            deserialize_helper<T>::deserialize_template(s, item, offset);
        }
    }
};


template <class T, class U>
struct serialize_helper<std::map<T, U>> {
    static auto serialize_template(std::map<T, U> &t, std::string &s, std::size_t &offset) {
        std::size_t size = t.size();
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        for (auto &pair : t) {
            T fi = pair.first;
            serialize_helper<T>::serialize_template(fi, s, offset);
            serialize_helper<U>::serialize_template(pair.second, s, offset);
        }
        return ;
    }
};
template <class T, class U>
struct deserialize_helper<std::map<T, U>> {
    static auto deserialize_template(std::string &s, std::map<T, U> &t, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        for (std::size_t index = 0; index < size; index++) {
            T fi;
            deserialize_helper<T>::deserialize_template(s, fi, offset);
            U se;
            deserialize_helper<U>::deserialize_template(s, se, offset);
            t[fi] = se;
        }
        return ;
    }
};

}