#pragma once

#include "otas_macro.h"
#include "otas_reflection.h"
#include <iostream>

namespace otas_serializer {

template <class T>
concept to_string_type = requires(T container) {
    { std::to_string(container) } -> std::same_as<std::string>;
};

void search(std::string buffer, size_t &offset, const char c) {
    while (buffer[offset] != c) {
        offset++;
    }
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
        } else if constexpr (map_container<T>) {
            auto it = t.begin();
            buffer.append("[");
            if (it != t.end()) {
                buffer.append("{\"key\":");
                json_serialize_helper<typename T::key_type>::serialize_template(it->first, buffer);
                buffer.append(",\"value\":");
                json_serialize_helper<typename T::mapped_type>::serialize_template(it->second, buffer);
                buffer.append("}");
                it++;
            }
            for (;it != t.end(); it++) {
                buffer.append(",");
                buffer.append("{\"key\":");
                json_serialize_helper<typename T::key_type>::serialize_template(it->first, buffer);
                buffer.append(",\"value\":");
                json_serialize_helper<typename T::mapped_type>::serialize_template(it->second, buffer);
                buffer.append("}");
            }
            buffer.append("]");
        }        
        else if constexpr (vector_container<T> || set_container<T> || list_container<T>) {
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
                    buffer.append(index == 0 ? "\"" : ",\n\""),
                    buffer.append(names[index]), buffer.append("\":"),
                    json_serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), buffer)
                ), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};

/*
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
            while (true) {
                if (buffer[offset] == '-') {
                    t = -t;
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
        } else if constexpr (map_container<T>) {
        }        
        else if constexpr (vector_container<T> || set_container<T> || list_container<T>) {
            search('[');
            json_deserialize_helper<typename T::value_type>::deserialize_template(buffer, *it, offset);
            it++;
            for (;it != t.end(); it++) {
                offset++;
                json_deserialize_helper<typename T::value_type>::deserialize_template(buffer, *it, offset);
            }
            offset++;
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            auto names =  member_name_helper<T>::tuple_name();
            [&]<std::size_t... index>(std::index_sequence<index...>) {
                ((  
                    offset += index == 0 ? 1 : 3, 
                    offset += names[index].length(),
                    offset += 2,
                    json_deserialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::deserialize_template(buffer, std::get<index>(members), offset)
                ), ...);
            } (std::make_index_sequence<count>{});
        }
    }
};

*/

template <class T>
auto serialize_json(const T &obj) {
    std::string buffer;
    buffer.append("{\n");
    json_serialize_helper<T>::serialize_template(obj, buffer);
    buffer.append("\n}");
    return buffer;
};

/*
template <class T> 
auto deserialize_json(const std::string &buffer) {
    T obj;
    std::size_t offset{};
    offset += 2;
    json_deserialize_helper<T>::deserialize_template(buffer, obj, offset);
    offset += 2;
    return obj;
};
*/

}