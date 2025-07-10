# otas_serialization

Header-only serializer. No need any additional code. 

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
- C++17 or above

- all basic types(int, float, double, char...)
- aggregate type(no consturctor function, no private members)
- most of STL containers

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
- others(TODO...)

## How does it work?
To build a non-invasive serializer, we need static reflection in C++. The fisrt question is how to get the number of members in a struct during compiling. One possible solution is list initialization in *C++11*. We could try to construct a object with one element, two elements, three elements... Like this:
```cpp
T{{}};
T{{},{}};
T{{},{},{}};
```
If the number of elements is less or equal to the number of struct members, constructor will succeed. Or it will fail. So when it fails, we get the number of struct members.

But how can we calculate it during compiling? Now we need `std::void_t` in *C++17*. It can determine whether the constructor mentioned above is successful. Combined with *template recursion*, *variadic parameters*, *decltype*, *sizeof*, we can get the number of members in a struct.

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
In this code block, `std::void_t<decltype(T{Args{}...})>` could tell compiler if constructor fucntion succeeds. 

Then we need to get each member from a struct. Here we can use *structure binding* in C++17. Finally we get:
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
```
Well, a little dirty, but useful.

## Future plans
- Support more STL containers
- Support json
- Increase efficienvy

## Need help
- If you know how to increase efficiency of a serializer, please tell me.
- I'm not a native english speaker, if you find any grammar error in this documnet or just read no smoothly, please point them out.
