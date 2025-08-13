#pragma once

#include "otas_reflection.h"

namespace otas_serializer {

constexpr unsigned int code_generate(const std::string_view &s) {
    unsigned int code{};
    for (int i = 0; i < s.size(); i++) {
        code *= 998244353;
        code += s[i] * 9982443533;
    }
    return code;
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
        if constexpr (map_container<T>) {
            code *= 998244353;
            code += check_code_helper<typename T::key_type>::value;
            code *= 998244353;
            code += check_code_helper<typename T::mapped_type>::value;
        } else if constexpr (normal_container<T>) {
            code *= 998244353;
            code += check_code_helper<typename T::value_type>::value;
        } else if constexpr (!std::is_arithmetic_v<T> && !std::is_same_v<std::string, T> && !std::is_same_v<std::wstring, T>) {
            constexpr auto count = get_member_count<T>();
            if constexpr (count != 0) {
                auto members = member_tuple_helper<T, count>::static_tuple_view();
                [&]<std::size_t... index>(std::index_sequence<index...>) {
                    ((code *= 998244353, code += check_code_helper<std::remove_pointer_t<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>>::value), ...);
                } (std::make_index_sequence<count>{});
            }
        } 
        return code;
    }
};

template <class T, class U>
struct check_code_struct<std::pair<T, U>> {
    constexpr static unsigned int check_code_template() {
        unsigned int code = code_generate(type_name<std::pair<T, U>>());
        code *= 998244353;
        code += check_code_helper<T>::value;
        code *= 998244353;
        code += check_code_helper<U>::value;
        return code;
    }
};

template <class T, std::size_t N>
struct check_code_struct<std::array<T, N>> {
    constexpr static unsigned int check_code_template() {
        unsigned int code = code_generate(type_name<std::array<T, N>>());
        code *= 998244353;
        code += check_code_helper<T>::value;
        code *= 998244353;
        code += N;
        return code;
    }
};

template <class ...Args>
struct check_code_struct<std::tuple<Args...>> {
    constexpr static unsigned int check_code_template() {
        unsigned int code = code_generate(type_name<std::tuple<Args...>>());
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((code *= 998244353, code += check_code_helper<remove_cvref_t<Args>>::value), ...);
        } (std::make_index_sequence<sizeof...(Args)>{});
        return code;
    }
};

template <class ...Args>
struct check_code_struct<std::variant<Args...>> {
    constexpr static unsigned int check_code_template() {
        unsigned int code = code_generate(type_name<std::variant<Args...>>);
        [&]<std::size_t... index>(std::index_sequence<index...>) {
            ((code *= 998244353, code += check_code_helper<remove_cvref_t<Args>>::value), ...);
        } (std::make_index_sequence<sizeof...(Args)>{});
        return code;
    }
};


}