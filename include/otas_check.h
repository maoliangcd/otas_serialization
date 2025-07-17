#pragma once

#include "otas_reflection.h"
#include <iostream>
#include <cstring>
#include <cxxabi.h>

namespace otas_serializer {

inline static const unsigned int md5_state[4]{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

constexpr unsigned int code_generate(const std::string_view &s) {
    unsigned int code{};
    for (int i = 0; i < s.size(); i++) {
        code *= md5_state[i % 4];
        code += s[i] * md5_state[i % 4];
    }
    return code;
}

template <class T>
constexpr std::string_view type_name() {
    std::string_view name = __PRETTY_FUNCTION__;
    return name;
}

template <class T>
struct check_code_struct;

template <class T>
struct check_code_helper {
    inline static constexpr unsigned int value = check_code_struct<T>::check_code_template();
};

template <class T>
struct check_code_struct {
    constexpr static unsigned int check_code_template() {
        unsigned int code = code_generate(type_name<T>());
        // constexpr auto count = get_member_count<T>();
        // if constexpr (count != 0) {
        //     auto members = member_tuple_helper<T, count>::static_tuple_view();
        //     [&]<std::size_t... index>(std::index_sequence<index...>) {
        //         ((code ^= check_code_helper<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>::value), ...);
        //     } (std::make_index_sequence<count>{});
        // }
        return code;
    }
};


}