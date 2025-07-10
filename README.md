# otas_serialization

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![Generic badge](https://img.shields.io/badge/C++-17-blue.svg)](https://shields.io/)


A header-only serialization tool. No configuration file, no additional define. 

## A quick start:
```cpp
#include <iostream>
#include <string>
#include "otas_serializer.h"

struct Leaf {
    double x;
};
struct Otas {
    int a;
    Leaf b;
};

int main() {
    Otas otas1{1, {2.0}};
    std::string s;
    otas_serializer::serialize(otas1, s);
    Otas otas2;
    otas_serializer::deserialize(s, otas2);
    std::cout << otas2.a << " " << otas2.b.x << std::endl;
    system("pause");
    return 0;
}
```

Yes! You don't need to add any `define` or configuration file to your code! Just apply `otas_serializer::serialize` and `otas_serializer::deserialize` to your aggregate type.

## Supported type
- all basic types(int, float, double, char...)
- any aggregate type(no consturctor function, no private members)
- most of the STL containers

Support these STL containers:
- `std::string`
- `std::vector`
- `std::map`
- `std::unordered_map`
- `std::multimap`
- `std::unordered_multimap`
- `std::set`
- `std::unordered_set`
- `std::multiset`
- `std::unordered_multiset`
- `std::queue`
- `std::deque`
- `std::list`
- `std::array`
- `std::optional`
- `std::pair`
- `others(todo)`

## How does it work?
You may know reflection of programming language. Reflection is a technique that enables obtaining struct information during compilation or runtime. Unfortunately, C++ doesn't support dynamic reflection and is weak in static reflection. However, it is already enough to build a non-invasive serializer with C++17.

The fisrt question is how to get the number of members in a struct during compilation. Here we use list initialization since *C++11*. We could try to construct a struct with one element, two elements, three elements. If the number of elements is less or equal to the number of struct members, constructor will succeed. Or it will fail. So when it fails, we get the number of struct members. A simple example:
```cpp
class T{
    int a;
    int b;
};

T t1{{}};
T t2{{},{}};
T t3{{},{},{}};
```
In this example, the first and the second constructor succeed, the third one fails. 

But how does compilier know if constructor succeed? Now we need `std::void_t<T>` since *C++17*. If `T` is a valid type, it returns `void`, or it triggers SFINAE. Combined with *variadic parameters*, *decltype*, we can get code `std::void_t<decltype(T{Args{}...})>`. 
```cpp
struct any_type {
    template <typename T>
    operator T();
};
template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T, typename = void, typename ...Args>
struct member_count_struct {
    constexpr static size_t value = sizeof...(Args) - 1;
};
template <typename T, typename ...Args>
struct member_count_struct<T, std::void_t<decltype(T{{Args{}}...})>, Args...> {
    constexpr static size_t value = member_count_struct<T, void, Args..., any_type>::value;
};
```

Finally we need to extract each element from a struct. A useful tool is *structure binding* since C++17. However, it needs some hard code.
```cpp
#define GENERATE_TEMPLATE(n, ...) \
template <class T> \
struct member_tuple_helper<T, n> { \
    inline constexpr static auto tuple_view(T &t) { \
        auto &&[__VA_ARGS__] = t; \
        return std::tie(__VA_ARGS__); \
    } \
    inline constexpr static auto tuple_view(const T &t) { \
        auto &&[__VA_ARGS__] = t; \
        return std::tie(__VA_ARGS__); \
    } \
} \

GENERATE_TEMPLATE(1, f0);
GENERATE_TEMPLATE(2, f0, f1);
GENERATE_TEMPLATE(3, f0, f1, f2);
GENERATE_TEMPLATE(4, f0, f1, f2, f3);
GENERATE_TEMPLATE(5, f0, f1, f2, f3, f4);
GENERATE_TEMPLATE(6, f0, f1, f2, f3, f4, f5);
GENERATE_TEMPLATE(7, f0, f1, f2, f3, f4, f5, f6);
GENERATE_TEMPLATE(8, f0, f1, f2, f3, f4, f5, f6, f7);
```
Well, a little dirty, but useful.

## Future plans
- Support more STL containers
- Support json, xml
- Increase efficienvy

## Need help
- If you know how to increase efficiency of a serializer, please tell me.