#include <tuple>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>

#include "otas_reflection.h"

namespace otas_serializer {

template <class T>
struct serialize_helper {
    static auto serialize_template(const T &t, std::string &s, std::size_t &offset) {
        constexpr auto count = get_member_count<T>();
        auto members = member_tuple_helper<T, count>::tuple_view(t);
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), s, offset)), ...);
        } (std::make_index_sequence<count>{});
        return ;
    }
};

template <class T>
struct deserialize_helper {
    static auto deserialize_template(const std::string &s, T &t, std::size_t &offset) {
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
    static auto serialize_template(const type &t, std::string &s, std::size_t &offset) { \
        s.append(reinterpret_cast<char *>(const_cast<type *>(&t)), sizeof(t)); \
        offset += sizeof(t); \
        return ; \
    } \
}; \
template <> \
struct deserialize_helper<type> { \
    static auto deserialize_template(const std::string &s, type &t, std::size_t &offset) { \
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


template <class T>
struct serialize_helper<std::vector<T>> {
    static auto serialize_template(const std::vector<T> &vec, std::string &s, std::size_t &offset) {
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
    static auto deserialize_template(const std::string &s, std::vector<T> &vec, std::size_t &offset) {
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
    static auto serialize_template(const std::map<T, U> &t, std::string &s, std::size_t &offset) {
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
    static auto deserialize_template(const std::string &s, std::map<T, U> &t, std::size_t &offset) {
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


template <class T, class U>
struct serialize_helper<std::unordered_map<T, U>> {
    static auto serialize_template(const std::unordered_map<T, U> &t, std::string &s, std::size_t &offset) {
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
struct deserialize_helper<std::unordered_map<T, U>> {
    static auto deserialize_template(const std::string &s, std::unordered_map<T, U> &t, std::size_t &offset) {
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

template <class T>
struct serialize_helper<std::set<T>> {
    static auto serialize_template(const std::set<T> &t, std::string &s, std::size_t &offset) {
        std::size_t size = t.size();
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        for (auto &item : t) {
            T obj = item;
            serialize_helper<T>::serialize_template(obj, s, offset);
        }
        return ;
    }
};
template <class T>
struct deserialize_helper<std::set<T>> {
    static auto deserialize_template(const std::string &s, std::set<T> &t, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        for (std::size_t index = 0; index < size; index++) {
            T item;
            deserialize_helper<T>::deserialize_template(s, item, offset);
            t.insert(item);
        }
        return ;
    }
};

template <class T>
struct serialize_helper<std::unordered_set<T>> {
    static auto serialize_template(const std::unordered_set<T> &t, std::string &s, std::size_t &offset) {
        std::size_t size = t.size();
        s.append(reinterpret_cast<char *>(&size), sizeof(size));
        offset += sizeof(size);
        for (auto &item : t) {
            T obj = item;
            serialize_helper<T>::serialize_template(obj, s, offset);
        }
        return ;
    }
};
template <class T>
struct deserialize_helper<std::unordered_set<T>> {
    static auto deserialize_template(const std::string &s, std::unordered_set<T> &t, std::size_t &offset) {
        std::size_t size;
        memcpy(&size, &s[offset], sizeof(size));
        offset += sizeof(size);
        for (std::size_t index = 0; index < size; index++) {
            T item;
            deserialize_helper<T>::deserialize_template(s, item, offset);
            t.insert(item);
        }
        return ;
    }
};


}