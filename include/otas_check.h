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
        if constexpr (!std::is_arithmetic_v<T> && !std::is_same_v<std::string, T> && !std::is_same_v<std::wstring, T>) {
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

#define GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(type) \
template <class T> \
struct check_code_struct<type<T>> { \
    constexpr static unsigned int check_code_template() { \
        unsigned int code = code_generate(type_name<type<T>>()); \
        code *= 998244353; \
        code += check_code_helper<T>::value; \
        return code; \
    } \
}

GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::vector);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::list);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::queue);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::deque);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::set);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::unordered_set);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::multiset);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::unordered_multiset);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::optional);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::unique_ptr);
GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE(std::forward_list);


#undef GENERATE_CHECK_CODE_ONE_CLASS_TEMPLATE

#define GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE(type) \
template <class T, class U> \
struct check_code_struct<type<T, U>> { \
    constexpr static unsigned int check_code_template() { \
        unsigned int code = code_generate(type_name<type<T, U>>()); \
        code *= 998244353; \
        code += check_code_helper<T>::value; \
        code *= 998244353; \
        code += check_code_helper<U>::value; \
        return code; \
    } \
}

GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE(std::map);
GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE(std::unordered_map);
GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE(std::multimap);
GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE(std::unordered_multimap);
GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE(std::pair);

#undef GENERATE_CHECK_CODE_TWO_CLASS_TEMPLATE


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