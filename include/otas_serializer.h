#pragma once

#include <tuple>
#include <cstdint>
#include <cassert>

#include "otas_macro.h"
#include "otas_reflection.h"
#include "otas_check.h"
#include <iostream>
namespace otas_serializer {

template <class T>
concept map_container = requires(T container) {
    typename T::key_type;
    typename T::mapped_type;
    container.size();
    container.begin();
    container.end();
};

template <class T>
concept set_container = requires(T container) {
    typename T::key_type;
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
};

template <class T>
concept list_container = requires(T container) {
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
};

template <class T, class Buffer, bool copy>
struct serialize_helper {
    ALWAYS_INLINE static auto serialize_template(const T &t, Buffer &s, std::size_t &offset) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            if constexpr (copy) {
                memcpy(&s[offset], &t, sizeof(t));
            }
            offset += sizeof(t);
        } else if constexpr (map_container<T>) {
            unsigned int size = t.size();
            if constexpr (copy) {
                memcpy(&s[offset], &size, sizeof(size));
            }
            offset += sizeof(size);
            for (const auto &pair : t) {
                serialize_helper<typename T::key_type, Buffer, copy>::serialize_template(pair.first, s, offset);
                serialize_helper<typename T::mapped_type, Buffer, copy>::serialize_template(pair.second, s, offset);
            }
        } else if constexpr (set_container<T>) {
            unsigned int size = t.size();
            if constexpr (copy) {
                memcpy(&s[offset], &size, sizeof(size));
            }
            offset += sizeof(size);
            for (const auto &item : t) {
                serialize_helper<typename T::key_type, Buffer, copy>::serialize_template(item, s, offset);
            }
        } else if constexpr (list_container<T>) {
            unsigned int size = t.size();
            if constexpr (copy) {
                memcpy(&s[offset], &size, sizeof(size));
            }
            offset += sizeof(size);
            for (const auto &item : t) {
                serialize_helper<typename T::value_type, Buffer, copy>::serialize_template(item, s, offset);
            }
        }
        else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>, Buffer, copy>::serialize_template(std::get<index>(members), s, offset)), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};

template <class T, class Buffer>
struct deserialize_helper {
    static auto deserialize_template(const Buffer &s, T &t, std::size_t &offset) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            memcpy(&t, &s[offset], sizeof(t));
            offset += sizeof(t);
        } else if constexpr (map_container<T>) {
            unsigned int size;
            memcpy(&size, &s[offset], sizeof(size));
            offset += sizeof(size);
            for (unsigned int index = 0; index < size; index++) {
                typename T::key_type fi;
                deserialize_helper<typename T::key_type, Buffer>::deserialize_template(s, fi, offset);
                typename T::mapped_type se;
                deserialize_helper<typename T::mapped_type, Buffer>::deserialize_template(s, se, offset);
                t.emplace(fi, se);
            }
        } else if constexpr (set_container<T>) {
            unsigned int size;
            memcpy(&size, &s[offset], sizeof(size));
            offset += sizeof(size);
            for (unsigned int index = 0; index < size; index++) { \
                typename T::key_type item;
                deserialize_helper<typename T::key_type, Buffer>::deserialize_template(s, item, offset);
                t.emplace(item);
            }
        } else if constexpr (list_container<T>) {
            unsigned int size;
            memcpy(&size, &s[offset], sizeof(size));
            offset += sizeof(size);
            t.resize(size);
            for (auto &item : t) {
                deserialize_helper<typename T::value_type, Buffer>::deserialize_template(s, item, offset);
            }
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((deserialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>, Buffer>::deserialize_template(s, std::get<index>(members), offset)), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};


template <class Buffer, bool copy>
struct serialize_helper<std::string, Buffer, copy> {
    static auto serialize_template(const std::string &t, Buffer &s, std::size_t &offset) {
        unsigned int size = t.length();
        if constexpr (copy) {
            memcpy(&s[offset], &size, sizeof(size));
        }
        offset += sizeof(size);
        if constexpr (copy) {
            memcpy(&s[offset], t.c_str(), size);
        }
        offset += size;
        return ;
    }
};
template <class Buffer>
struct deserialize_helper<std::string, Buffer> {
    static auto deserialize_template(const Buffer &s, std::string &t, std::size_t &offset) {
        unsigned int size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        t = std::string(&s[offset], size);
        offset += size;
        return ;
    }
};


template <class Buffer, bool copy>
struct serialize_helper<std::wstring, Buffer, copy> {
    static auto serialize_template(const std::wstring &t, Buffer &s, std::size_t &offset) {
        unsigned int size = t.length() * sizeof(wchar_t);
        if constexpr (copy) {
            memcpy(&s[offset], &size, sizeof(size));
        }
        offset += sizeof(size);
        if constexpr (copy) {
            memcpy(&s[offset], t.c_str(), size);
        }
        offset += size;
        return ;
    }
};
template <class Buffer>
struct deserialize_helper<std::wstring, Buffer> {
    static auto deserialize_template(const Buffer &s, std::wstring &t, std::size_t &offset) {
        unsigned int size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        t = std::wstring(reinterpret_cast<wchar_t *>(const_cast<char *>(&s[offset])), size / sizeof(wchar_t));
        offset += size;
        return ;
    }
};


template <class T, class Buffer, bool copy>
struct serialize_helper<std::vector<T>, Buffer, copy> {
    static auto serialize_template(const std::vector<T> &t, Buffer &s, std::size_t &offset) {
        unsigned int size = t.size();
        if constexpr (copy) {
            memcpy(&s[offset], &size, sizeof(size));
        }
        offset += sizeof(size);
        if constexpr (std::is_trivially_copyable_v<T>) {
            if constexpr (copy) {
                memcpy(&s[offset], t.data(), size * sizeof(T));
            }
            offset += size * sizeof(T);
        } else {
            for (unsigned int index = 0; index < size; index++) {
                serialize_helper<T, Buffer, copy>::serialize_template(t[index], s, offset);
            }
        }
        return ;
    }
};
template <class T, class Buffer>
struct deserialize_helper<std::vector<T>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::vector<T> &t, std::size_t &offset) {
        unsigned int size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        t.resize(size);
        if constexpr (std::is_trivially_copyable_v<T>) {
            memcpy(t.data(), &s[offset], sizeof(T) * size);
            offset += sizeof(T) * size;
        } else {
            for (unsigned int index = 0; index < size; index++) {
                deserialize_helper<T, Buffer>::deserialize_template(s, t[index], offset);
            }
        }
    }
};


#define GENERATE_TEMPLATE_ITERATOR_TYPE(type) \
template <class T, class Buffer, bool copy> \
struct serialize_helper<type<T>, Buffer, copy> { \
    static auto serialize_template(const type<T> &t, Buffer &s, std::size_t &offset) { \
        unsigned int size = t.size(); \
        if constexpr (copy) { \
            memcpy(&s[offset], &size, sizeof(size)); \
        } \
        offset += sizeof(size); \
        for (const auto &item : t) { \
            serialize_helper<T, Buffer, copy>::serialize_template(item, s, offset); \
        } \
        return ; \
    } \
}; \
template <class T, class Buffer> \
struct deserialize_helper<type<T>, Buffer> { \
    static auto deserialize_template(const Buffer &s, type<T> &t, std::size_t &offset) { \
        unsigned int size; \
        memcpy(&size, &s[offset], sizeof(size)); \
        offset += sizeof(size); \
        t.resize(size); \
        for (auto &item : t) { \
            deserialize_helper<T, Buffer>::deserialize_template(s, item, offset); \
        } \
    } \
} \

GENERATE_TEMPLATE_ITERATOR_TYPE(std::list);
GENERATE_TEMPLATE_ITERATOR_TYPE(std::queue);
// GENERATE_TEMPLATE_ITERATOR_TYPE(std::deque);


template <class T, std::size_t N, class Buffer, bool copy>
struct serialize_helper<std::array<T, N>, Buffer, copy> {
    static auto serialize_template(const std::array<T, N> &t, Buffer &s, std::size_t &offset) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            if constexpr (copy) {
                memcpy(&s[offset], t.data(), N * sizeof(T));
            }
            offset += N * sizeof(T);
        } else {
            for (unsigned int index = 0; index < N; index++) {
                serialize_helper<T, Buffer, copy>::serialize_template(t[index], s, offset);
            }
        }
        return ;
    }
};
template <class T, std::size_t N, class Buffer>
struct deserialize_helper<std::array<T, N>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::array<T, N> &t, std::size_t &offset) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            memcpy(t.data(), sizeof(T) * N);
            offset += sizeof(T) * N;
        } else {
            for (unsigned int index = 0; index < N; index++) {
                deserialize_helper<T, Buffer>::deserialize_template(s, t[index], offset);
            }
        }
        return ;
    }
};


template <class T, class Buffer, bool copy>
struct serialize_helper<std::optional<T>, Buffer, copy> {
    static auto serialize_template(const std::optional<T> &t, Buffer &s, std::size_t &offset) {
        bool exist = t.has_value();
        if constexpr (copy) {
            memcpy(&s[offset], &exist, sizeof(exist));
        }
        offset += sizeof(exist);
        if (exist) {
            serialize_helper<T, Buffer, copy>::serialize_template(t.value(), s, offset);
        }
        return ;
    }
};
template <class T,class Buffer>
struct deserialize_helper<std::optional<T>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::optional<T> &t, std::size_t &offset) {
        bool exist;
        memcpy(&exist, &s[offset], sizeof(exist));
        offset += sizeof(exist);
        if (exist) {
            t = T{};
            deserialize_helper<T, Buffer>::deserialize_template(s, t.value(), offset);
        }
        return ;
    }
};


template <class T, class U, class Buffer, bool copy>
struct serialize_helper<std::pair<T, U>, Buffer, copy> {
    static auto serialize_template(const std::pair<T, U> &t, Buffer &s, std::size_t &offset) {
        serialize_helper<T, Buffer, copy>::serialize_template(t.first, s, offset);
        serialize_helper<U, Buffer, copy>::serialize_template(t.second, s, offset);
        return ;
    }
};
template <class T, class U, class Buffer>
struct deserialize_helper<std::pair<T, U>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::pair<T, U> &t, std::size_t &offset) {
        deserialize_helper<T, Buffer>::deserialize_template(s, t.first, offset);
        deserialize_helper<T, Buffer>::deserialize_template(s, t.second, offset);
        return ;
    }
};


template <class T, class Buffer, bool copy>
struct serialize_helper<std::unique_ptr<T>, Buffer, copy> {
    static auto serialize_template(const std::unique_ptr<T> &t, Buffer &s, std::size_t &offset) {
        bool exist = t.get();
        if constexpr (copy) {
            memcpy(&s[offset], &exist, sizeof(exist));
        }
        offset += sizeof(exist);
        if (exist) {
            serialize_helper<T, Buffer, copy>::serialize_template(*t, s, offset);
        }
        return ;
    }
};
template <class T, class Buffer>
struct deserialize_helper<std::unique_ptr<T>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::unique_ptr<T> &t, std::size_t &offset) {
        bool exist;
        memcpy(&exist, &s[offset], sizeof(exist));
        offset += sizeof(exist);
        if (exist) {
            T* ptr = new T{};
            deserialize_helper<T, Buffer>::deserialize_template(s, *ptr, offset);
            t.reset(ptr);
        }
        return ;
    }
};


template <class T, class Buffer, bool copy>
struct serialize_helper<std::forward_list<T>, Buffer, copy> {
    static auto serialize_template(const std::forward_list<T> &t, Buffer &s, std::size_t &offset) {
        unsigned int size{};
        auto offset_back = offset;
        if constexpr (copy) {
            memcpy(&s[offset], &size, sizeof(size));
        }
        offset += sizeof(size);
        for (const auto &item : t) {
            size++;
            serialize_helper<T, Buffer, copy>::serialize_template(item, s, offset);
        }
        if constexpr (copy) {
            memcpy(&s[offset_back], &size, sizeof(size));
        }
        return ;
    }
};
template <class T, class Buffer>
struct deserialize_helper<std::forward_list<T>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::forward_list<T> &t, std::size_t &offset) {
        unsigned int size{};
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        for (unsigned int index = 0; index < size; index++) {
            t.push_front(T{});
        }
        for (auto &item : t) {
            deserialize_helper<T, Buffer>::deserialize_template(s, item, offset);
        }
        return ;
    }
};


template <class ...Args, class Buffer, bool copy>
struct serialize_helper<std::tuple<Args...>, Buffer, copy> {
    static auto serialize_template(const std::tuple<Args...> &t, Buffer &s, std::size_t &offset) {
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((serialize_helper<remove_cvref_t<std::tuple_element_t<index, std::tuple<Args...>>>, Buffer, copy>::serialize_template(std::get<index>(t), s, offset)), ...);
        } (std::make_index_sequence<sizeof...(Args)>{});
        return ;
    }
};
template <class ...Args, class Buffer>
struct deserialize_helper<std::tuple<Args...>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::tuple<Args...> &t, std::size_t &offset) {
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((deserialize_helper<remove_cvref_t<std::tuple_element_t<index, std::tuple<Args...>>>, Buffer>::deserialize_template(s, std::get<index>(t), offset)), ...);
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


template <class ...Args, class Buffer, bool copy>
struct serialize_helper<std::variant<Args...>, Buffer, copy> {
    static auto serialize_template(const std::variant<Args...> &t, Buffer &s, std::size_t &offset) {
        unsigned int index = t.index();
        memcpy(&s[offset], &index, sizeof(index));
        offset += sizeof(index);
        std::visit([&](const auto &value) {
            serialize_helper<remove_cvref_t<decltype(value)>, Buffer, copy>::serialize_template(value, s, offset);
        }, t);
        return ;
    }
};
template <class ...Args, class Buffer>
struct deserialize_helper<std::variant<Args...>, Buffer> {
    static auto deserialize_template(const Buffer &s, std::variant<Args...> &t, std::size_t &offset) {
        unsigned int index;
        memcpy(&index, &s[offset], sizeof(index));
        offset += sizeof(index);
        switch_variant_type<std::variant<Args...>>(t, index);
        std::visit([&](auto &value) {
            deserialize_helper<remove_cvref_t<decltype(value)>, Buffer>::deserialize_template(s, value, offset);
        }, t);
        return ;
    }
};

#undef GENERATE_TEMPLATE_ITERATOR_TYPE
#undef GENERATE_TEMPLATE_MAP_TYPE
#undef GENERATE_TEMPLATE_CONTAINER_INSERT_TYPE
#undef GENERATE_TEMPLATE_SMART_PTR_TYPE

struct otas_buffer {
    char *data_{};
    unsigned int size_{};

    char *data() const {
        return data_;
    }
    void resize(int size) {
        delete[] data_;
        data_ = new char[size];
        size_ = size;
        return ;
    }
};

template <class Buffer = std::string, class T>
auto serialize(const T &obj) {
    Buffer buffer;
    using type = decltype(buffer.data());
    type data = buffer.data();
    std::size_t size{4};
    serialize_helper<T, type, false>::serialize_template(obj, data, size);
    buffer.resize(size);
    data = buffer.data();
    auto check_code = check_code_helper<T>::value;
    memcpy(data, &check_code, 4);
    std::size_t offset{4};
    serialize_helper<T, type, true>::serialize_template(obj, data, offset);
    return buffer;
};

template <class T, class Buffer>
auto deserialize(const Buffer &buffer) {
    using type = decltype(buffer.data());
    type data = buffer.data();
    unsigned int check_code;
    memcpy(&check_code, data, 4);
    std::size_t offset{4};
    T obj{};
    if (check_code != check_code_helper<T>::value) {
        return obj;
    }
    deserialize_helper<T, type>::deserialize_template(data, obj, offset);
    return obj;
};

}