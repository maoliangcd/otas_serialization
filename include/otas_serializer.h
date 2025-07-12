#include <tuple>
#include <cstdint>
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

#include "otas_reflection.h"
namespace otas_serializer {

template <class T>
struct serialize_helper {
    static auto serialize_template(const T &t, std::string &s, std::size_t &offset) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            s.append(reinterpret_cast<char *>(const_cast<T *>(&t)), sizeof(t));
            offset += sizeof(t);
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), s, offset)), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};

template <class T>
struct deserialize_helper {
    static auto deserialize_template(const std::string &s, T &t, std::size_t &offset) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            memcpy(&t, &s[offset], sizeof(t));
            offset += sizeof(t);
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((deserialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::deserialize_template(s, std::get<index>(members), offset)), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};

template <>
struct serialize_helper<std::string> {
    static auto serialize_template(const std::string &t, std::string &s, std::size_t &offset) {
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
    static auto deserialize_template(const std::string &s, std::string &t, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        t = std::string(&s[offset], size);
        offset += size;
        return ;
    }
};

#define GENERATE_TEMPLATE_ITERATOR_TYPE(type) \
template <class T> \
struct serialize_helper<type<T>> { \
    static auto serialize_template(const type<T> &t, std::string &s, std::size_t &offset) { \
        std::size_t size = t.size(); \
        s.append(reinterpret_cast<char *>(&size), sizeof(size)); \
        offset += sizeof(size); \
        for (const auto &item : t) { \
            serialize_helper<T>::serialize_template(item, s, offset); \
        } \
        return ; \
    } \
}; \
template <class T> \
struct deserialize_helper<type<T>> { \
    static auto deserialize_template(const std::string &s, type<T> &t, std::size_t &offset) { \
        std::size_t size; \
        memcpy(&size, &s[offset], sizeof(size)); \
        offset += sizeof(size); \
        t.resize(size); \
        for (auto &item : t) { \
            deserialize_helper<T>::deserialize_template(s, item, offset); \
        } \
    } \
} \

GENERATE_TEMPLATE_ITERATOR_TYPE(std::vector);
GENERATE_TEMPLATE_ITERATOR_TYPE(std::list);
GENERATE_TEMPLATE_ITERATOR_TYPE(std::queue);
GENERATE_TEMPLATE_ITERATOR_TYPE(std::deque);


#define GENERATE_TEMPLATE_MAP_TYPE(type) \
template <class T, class U> \
struct serialize_helper<type<T, U>> { \
    static auto serialize_template(const type<T, U> &t, std::string &s, std::size_t &offset) { \
        std::size_t size = t.size(); \
        s.append(reinterpret_cast<char *>(&size), sizeof(size)); \
        offset += sizeof(size); \
        for (const auto &pair : t) { \
            serialize_helper<T>::serialize_template(pair.first, s, offset); \
            serialize_helper<U>::serialize_template(pair.second, s, offset); \
        } \
        return ; \
    } \
}; \
template <class T, class U> \
struct deserialize_helper<type<T, U>> { \
    static auto deserialize_template(const std::string &s, type<T, U> &t, std::size_t &offset) { \
        std::size_t size; \
        memcpy(&size, &s[offset], sizeof(size)); \
        offset += sizeof(size); \
        for (std::size_t index = 0; index < size; index++) { \
            T fi; \
            deserialize_helper<T>::deserialize_template(s, fi, offset); \
            U se; \
            deserialize_helper<U>::deserialize_template(s, se, offset); \
            t.emplace(fi, se); \
        } \
        return ;\
    } \
}

GENERATE_TEMPLATE_MAP_TYPE(std::map);
GENERATE_TEMPLATE_MAP_TYPE(std::unordered_map);
GENERATE_TEMPLATE_MAP_TYPE(std::multimap);
GENERATE_TEMPLATE_MAP_TYPE(std::unordered_multimap);


#define GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE(type) \
template <class T> \
struct serialize_helper<type<T>> { \
    static auto serialize_template(const type<T> &t, std::string &s, std::size_t &offset) { \
        std::size_t size = t.size(); \
        s.append(reinterpret_cast<char *>(&size), sizeof(size)); \
        offset += sizeof(size); \
        for (const auto &item : t) { \
            serialize_helper<T>::serialize_template(item, s, offset); \
        } \
        return ; \
    } \
}; \
template <class T> \
struct deserialize_helper<type<T>> { \
    static auto deserialize_template(const std::string &s, type<T> &t, std::size_t &offset) { \
        std::size_t size; \
        memcpy(&size, &s[offset], sizeof(size)); \
        offset += sizeof(size); \
        for (std::size_t index = 0; index < size; index++) { \
            T item; \
            deserialize_helper<T>::deserialize_template(s, item, offset); \
            t.emplace(item); \
        } \
        return ; \
    } \
} \

GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE(std::set);
GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE(std::unordered_set);
GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE(std::multiset);
GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE(std::unordered_multiset);

template <class T, std::size_t N>
struct serialize_helper<std::array<T, N>> {
    static auto serialize_template(const std::array<T, N> &t, std::string &s, std::size_t &offset) {
        std::size_t size = t.size();
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        for (const auto &item : t) {
            serialize_helper<T>::serialize_template(item, s, offset);
        }
        return ;
    }
};
template <class T, std::size_t N>
struct deserialize_helper<std::array<T, N>> {
    static auto deserialize_template(const std::string &s, std::array<T, N> &t, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        for (std::size_t index = 0; index < size; index++) {
            deserialize_helper<T>::deserialize_template(s, t[index], offset);
        }
        return ;
    }
};


template <class T>
struct serialize_helper<std::optional<T>> {
    static auto serialize_template(const std::optional<T> &t, std::string &s, std::size_t &offset) {
        bool exist = t.has_value();
        s.append(reinterpret_cast<char *>(&exist), sizeof(exist));
        offset += sizeof(exist);
        if (exist) {
            serialize_helper<T>::serialize_template(t.value(), s, offset);
        }
        return ;
    }
};
template <class T>
struct deserialize_helper<std::optional<T>> {
    static auto deserialize_template(const std::string &s, std::optional<T> &t, std::size_t &offset) {
        bool exist;
        memcpy(&exist, &s[offset], sizeof(exist));
        offset += sizeof(exist);
        if (exist) {
            t = T{};
            deserialize_helper<T>::deserialize_template(s, t.value(), offset);
        }
        return ;
    }
};


template <class T, class U>
struct serialize_helper<std::pair<T, U>> {
    static auto serialize_template(const std::pair<T, U> &t, std::string &s, std::size_t &offset) {
        serialize_helper<T>::serialize_template(t.first, s, offset);
        serialize_helper<U>::serialize_template(t.second, s, offset);
        return ;
    }
};
template <class T, class U>
struct deserialize_helper<std::pair<T, U>> {
    static auto deserialize_template(const std::string &s, std::pair<T, U> &t, std::size_t &offset) {
        deserialize_helper<T>::deserialize_template(s, t.first, offset);
        deserialize_helper<T>::deserialize_template(s, t.second, offset);
        return ;
    }
};

template <class T> \
struct serialize_helper<std::unique_ptr<T>> { \
    static auto serialize_template(const std::unique_ptr<T> &t, std::string &s, std::size_t &offset) { \
        bool exist = t.get(); \
        s.append(reinterpret_cast<char *>(&exist), sizeof(exist)); \
        offset += sizeof(exist); \
        if (exist) { \
            serialize_helper<T>::serialize_template(*t, s, offset); \
        } \
        return ; \
    } \
}; \
template <class T>
struct deserialize_helper<std::unique_ptr<T>> {
    static auto deserialize_template(const std::string &s, std::unique_ptr<T> &t, std::size_t &offset) {
        bool exist;
        memcpy(&exist, &s[offset], sizeof(exist));
        offset += sizeof(exist); \
        if (exist) {
            T* ptr = new T{};
            deserialize_helper<T>::deserialize_template(s, *ptr, offset);
            t.reset(ptr);
        }
        return ;
    }
};

template <class T>
struct serialize_helper<std::forward_list<T>> {
    static auto serialize_template(const std::forward_list<T> &t, std::string &s, std::size_t &offset) {
        std::size_t size{};
        auto offset_back = offset;
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        for (const auto &item : t) {
            size++;
            serialize_helper<T>::serialize_template(item, s, offset);
        }
        memcpy(&s[offset_back], &size, sizeof(size));
        return ;
    }
};
template <class T>
struct deserialize_helper<std::forward_list<T>> {
    static auto deserialize_template(const std::string &s, std::forward_list<T> &t, std::size_t &offset) {
        std::size_t size{};
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        for (std::size_t index = 0; index < size; index++) {
            t.push_front(T{});
        }
        for (auto &item : t) {
            deserialize_helper<T>::deserialize_template(s, item, offset);
        }
        return ;
    }
};


template <class ...Args>
struct serialize_helper<std::tuple<Args...>> {
    static auto serialize_template(const std::tuple<Args...> &t, std::string &s, std::size_t &offset) {
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((serialize_helper<remove_cvref_t<std::tuple_element_t<index, std::tuple<Args...>>>>::serialize_template(std::get<index>(t), s, offset)), ...);
        } (std::make_index_sequence<sizeof...(Args)>{});
        return ;
    }
};
template <class ...Args>
struct deserialize_helper<std::tuple<Args...>> {
    static auto deserialize_template(const std::string &s, std::tuple<Args...> &t, std::size_t &offset) {
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((deserialize_helper<remove_cvref_t<std::tuple_element_t<index, std::tuple<Args...>>>>::deserialize_template(s, std::get<index>(t), offset)), ...);
        } (std::make_index_sequence<sizeof...(Args)>{});
        return ;
    }
};

template <class T, std::size_t index>
struct switch_variant_type_helper {
    inline constexpr static void run(T &t) {
        if constexpr (index < std::variant_size_v<T>) {
            t = T{std::in_place_index_t<index>{}};
        }
        return ;
    }
};

#define GENERATE_VARIANT_CONSTRUCT_HELPER(index) \
case (index): \
if constexpr (index < std::variant_size_v<T>) { \
    return switch_variant_type_helper<T, index>::run(t); \
} else { \
    break; \
}

template <class T>
inline constexpr auto switch_variant_type(T &t, std::size_t index) {
    switch (index) {
        GENERATE_VARIANT_CONSTRUCT_HELPER(0);
        GENERATE_VARIANT_CONSTRUCT_HELPER(1);
        GENERATE_VARIANT_CONSTRUCT_HELPER(2);
        GENERATE_VARIANT_CONSTRUCT_HELPER(3);
        GENERATE_VARIANT_CONSTRUCT_HELPER(4);
        GENERATE_VARIANT_CONSTRUCT_HELPER(5);
        GENERATE_VARIANT_CONSTRUCT_HELPER(6);
        GENERATE_VARIANT_CONSTRUCT_HELPER(7);
        GENERATE_VARIANT_CONSTRUCT_HELPER(8);
        GENERATE_VARIANT_CONSTRUCT_HELPER(9);
        GENERATE_VARIANT_CONSTRUCT_HELPER(10);
        GENERATE_VARIANT_CONSTRUCT_HELPER(11);
        GENERATE_VARIANT_CONSTRUCT_HELPER(12);
        GENERATE_VARIANT_CONSTRUCT_HELPER(13);
        GENERATE_VARIANT_CONSTRUCT_HELPER(14);
        GENERATE_VARIANT_CONSTRUCT_HELPER(15);
        default:
            break;
    }
    return ;
}

#undef GENERATE_VARIANT_CONSTRUCT_HELPER

template <class ...Args>
struct serialize_helper<std::variant<Args...>> {
    static auto serialize_template(const std::variant<Args...> &t, std::string &s, std::size_t &offset) {
        std::size_t index = t.index();
        std::cout << index << std::endl;
        s.append(reinterpret_cast<char *>(&index), sizeof(index));
        offset += sizeof(index);
        std::visit([&](const auto &value) {
            serialize_helper<remove_cvref_t<decltype(value)>>::serialize_template(value, s, offset);
        }, t);
        return ;
    }
};
template <class ...Args>
struct deserialize_helper<std::variant<Args...>> {
    static auto deserialize_template(const std::string &s, std::variant<Args...> &t, std::size_t &offset) {
        std::size_t index;
        memcpy(&index, &s[offset], sizeof(index));
        offset += sizeof(index);
        switch_variant_type<std::variant<Args...>>(t, index);
        std::visit([&](auto &value) {
            deserialize_helper<remove_cvref_t<decltype(value)>>::deserialize_template(s, value, offset);
        }, t);
        return ;
    }
};

auto serialize = [](auto &&t, auto &&s) -> auto {
    std::size_t offset{};
    serialize_helper<remove_cvref_t<decltype(t)>>::serialize_template(t, s, offset);
};

auto deserialize = [](auto &&s, auto &&t) -> auto {
    std::size_t offset{};
    deserialize_helper<remove_cvref_t<decltype(t)>>::deserialize_template(s, t, offset);
};

#undef GENERATE_TEMPLATE_ITERATOR_TYPE
#undef GENERATE_TEMPLATE_MAP_TYPE
#undef GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE
#undef GENERATE_TEMPLATE_SMART_PTR_TYPE
}


// todo: std::in_place_t