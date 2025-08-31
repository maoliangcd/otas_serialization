#pragma once

#include "otas_macro.h"
#include "otas_reflection.h"
#include <iostream>

namespace otas_serializer {

template <class T>
concept to_string_type = requires(T container) {
    std::to_string(container);
};

void search(std::string buffer, size_t &offset, const char c) {
    while (buffer[offset++] != c);
    return ;
}

template <class T>
struct json_serialize_helper {
    ALWAYS_INLINE static auto serialize_template(const T &t, std::string &buffer) {
        if constexpr (char_container<T>) {
            buffer.append("\'");
            buffer.append(1, t);
            buffer.append("\'");
        } else if constexpr (string_container<T>) {
            buffer.append("\"");
            buffer.append(t.size(), t.data());
            buffer.append("\"");
        } else if constexpr (to_string_type<T>) {
            buffer.append(std::to_string(t));
        } else if constexpr (vector_container<T> || set_container<T> || list_container<T>) {
            auto it = t.begin();
            buffer.append("[");
            json_serialize_helper<typename T::value_type>::serialize_template(*it, buffer);
            it++;
            for (;it != t.end(); it++) {
                buffer.append(",");
                json_serialize_helper<typename T::value_type>::serialize_template(*it, buffer);
            }
            buffer.append("]");
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            auto names =  member_name_helper<T>::tuple_name();
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((
                    buffer.append(index == 0 ? "\"" : ",\""),
                    buffer.append(names[index]), buffer.append("\":"),
                    json_serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), buffer)
                ), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};

template <class T>
struct json_deserialize_helper {
    ALWAYS_INLINE static auto deserialize_template(const std::string &buffer, T &t, std::size_t &offset) {
        if constexpr (char_container<T>) {
            offset++;
            memcpy(&t, &buffer[offset], sizeof(t));
            offset += 2;
        } else if constexpr (string_container<T>) {
            offset++;
            memcpy(t.data(), &buffer[offset], t.size());
            offset += t.size() + 1;
        } else if constexpr (to_string_type<T>) {
            double pw{0.1};
            bool is_float{false};
            bool negative{false};
            while (true) {
                if (buffer[offset] == '-') {
                    negative = true;
                } else if (buffer[offset] == '.') {
                    is_float = true;
                } else if (buffer[offset] >= '0' && buffer[offset] <= '9') {
                    if (is_float) {
                        t += pw * (buffer[offset] - '0');
                        pw *= 0.1;
                    } else {
                        t *= 10;
                        t += buffer[offset] - '0';
                    }
                } else {
                    break;
                }
                offset++;
            }
            if (negative) {
                t = -t;
            }
        } else if constexpr (vector_container<T> || set_container<T> || list_container<T>) {
            search(buffer, offset,'[');
            while (buffer[offset] != ']') {
                typename T::value_type obj{};
                json_deserialize_helper<typename T::value_type>::deserialize_template(buffer, obj, offset);
                t.push_back(obj);
                if (buffer[offset] == ']') {
                    break;
                }
                offset++;
            }
            search(buffer, offset, ']');
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            auto names =  member_name_helper<T>::tuple_name();
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((  
                    search(buffer, offset, ':'),
                    json_deserialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::deserialize_template(buffer, std::get<index>(members), offset)
                ), ...);
            } (std::make_index_sequence<count>{});
        }
    }
};

template <class T>
auto serialize_json(const T &obj) {
    std::string buffer;
    buffer.append("{");
    json_serialize_helper<T>::serialize_template(obj, buffer);
    buffer.append("}");
    return buffer;
};

template <class T> 
auto deserialize_json(const std::string &buffer) {
    T obj;
    std::size_t offset{};
    search(buffer, offset, '{');
    json_deserialize_helper<T>::deserialize_template(buffer, obj, offset);
    search(buffer, offset, '}');
    return obj;
};


}