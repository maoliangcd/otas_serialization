#pragma once

#include "otas_macro.h"
#include "otas_reflection.h"
#include <iostream>

namespace otas_serializer {

template <class T, class Buffer, bool copy>
struct json_helper {
    ALWAYS_INLINE static auto serialize_template(const T &t, Buffer &s, std::size_t &offset) {
        if constexpr (vector_container<T>) {
            if constexpr (copy) {
                s[offset] = '[';
            }
            offset++;
            auto it = t.begin();
            json_helper<typename T::value_type, Buffer, copy>::serialize_template(*it, s, offset);
            it++;
            for (;it != t.end(); it++) {
                if constexpr (copy) {
                    s[offset] = ',';
                }
                offset++;
                json_helper<typename T::value_type, Buffer, copy>::serialize_template(*it, s, offset);
            }
            if constexpr (copy) {
                s[offset] = ']';
            }
            offset++;
        } else {
            constexpr auto count = get_member_count<T>();
            auto members = member_tuple_helper<T, count>::tuple_view(t);
            auto names =  member_name_helper<T>::tuple_name();

            if constexpr (copy) {
                [&]<std::size_t... index>(std::index_sequence<index...>) {
                    ((  
                        s[offset++] = '"',
                        memcpy(&s[offset], names[index].data(), names[index].length()),
                        offset += names[index].length(),
                        s[offset++] = '"',
                        s[offset++] = ':',
                        json_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>, Buffer, copy>::serialize_template(std::get<index>(members), s, offset)
                    ), ...);
                } (std::make_index_sequence<count>{});
            } else {
                [&]<std::size_t... index>(std::index_sequence<index...>) {
                    ((
                        offset += names[index].length() + 3,
                        json_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>, Buffer, copy>::serialize_template(std::get<index>(members), s, offset)
                    ), ...);
                } (std::make_index_sequence<count>{});
            }
        }
        return ;
    }
};

template <class Buffer, bool copy>
struct json_helper<int, Buffer, copy> {
    ALWAYS_INLINE static auto serialize_template(const int &t, Buffer &s, std::size_t &offset) {
        auto number = std::to_string(t);
        if constexpr (copy) {
            memcpy(&s[offset], number.data(), number.length());
        }
        offset += number.length();
        return ;
    }
};

template <class Buffer = std::string, class T>
auto serialize_json(const T &obj) {
    Buffer buffer;
    using type = decltype(buffer.data());
    type data = buffer.data();
    std::size_t size{};
    size++;
    json_helper<T, type, false>::serialize_template(obj, data, size);
    size++;
    buffer.resize(size);
    data = buffer.data();

    std::size_t offset{};
    data[offset++] = '{';
    json_helper<T, type, true>::serialize_template(obj, data, offset);
    data[offset++] = '}';

    return buffer;
};

}