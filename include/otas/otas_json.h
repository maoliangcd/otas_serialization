#pragma once

#include "otas_macro.h"
#include "otas_reflection.h"
#include <iostream>

namespace otas_serializer {

template <class T>
concept to_json_type = requires(T container, std::string buffer) {
    { container.to_json() } -> std::same_as<std::string>;
    container.from_json(buffer);
};

template <class T>
concept to_string_type = requires(T container) {
    { std::to_string(container) } -> std::same_as<std::string>;
};


template <class T>
struct json_serialize_helper {
    ALWAYS_INLINE static auto serialize_template(const T &t, std::string &buffer) {
        if constexpr (to_json_type<T>) {
            buffer.append(t.to_json());
        } else if constexpr (char_container<T>) {
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
                    buffer.append(std::get<index>(names)), buffer.append("\":"),
                    json_serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), buffer)
                ), ...);
            } (std::make_index_sequence<count>{});
        }
        return ;
    }
};

template <class T>
struct json_deserialize_helper {
    ALWAYS_INLINE static auto serialize_template(const T &t, std::string &buffer) {
    if constexpr (to_json_type<T>) {
            t.from_json();
        } else if constexpr (char_container<T>) {
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
                    buffer.append(std::get<index>(names)), buffer.append("\":"),
                    json_serialize_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::serialize_template(std::get<index>(members), buffer)
                ), ...);
            } (std::make_index_sequence<count>{});
        }
};



template <class Buffer = std::string, class T>
auto serialize_json(const T &obj) {
    Buffer buffer;
    buffer.append("{\n");
    json_serialize_helper<T>::serialize_template(obj, buffer);
    buffer.append("\n}");
    return buffer;
};

}