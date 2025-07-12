# otas_serialization

这是一个只包含头文件的序列化库。不需要额外的配置文件和宏定义。

## 快速开始:
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
没错，你不需要添加任何`define`，不需要编写配置文件。只需要使用在你的代码中调用`otas_serializer::serialize`和`otas_serializer::deserialize`。

## 支持的类型
- 基本类型(int, float, double, char...)
- 聚合类型(没有构造函数，没有私有成员)
- 大多数STL容器

支持以下STL容器：
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
- `std::unique_ptr`
- `std::forward_list`
- `others(todo)`

## 原理
你或许听说过编程中的反射。反射是一项能在编译或运行时获取结构体信息的技术。不幸的是，C++几乎不支持动态反射，也不怎么支持静态反射（据说C++26会全面支持，但是猴年马月）。不过，对于实现非侵入式序列化来说，C++17已经足够了。

第一个问题是如何在编译时就获得一个结构体的成员数量。此时可以使用*C++11*引入的初始化列表。尝试用一个变量，两个变量，三个变量···来构造一个结构体。如果变量的数量小于等于结构体成员数量，构造函数就会成功，否则就会失败。这里有一个简单的例子：

```cpp
class T{
    int a;
    int b;
};

T t1{{}};
T t2{{},{}};
T t3{{},{},{}};
```
在这个例子中，前两个构造会成功，第三个会失败。

但是编译器如何知道构造是否成功？下面需要用到*C++17*引入的`std::void_t<T>`。如果`T`是一个合法的类型，`std::void_t`将返回`void`，否则会触发*SFINAE*机制。再加上*可变参数*和*decltype*，我们就能得到这样一段代码`std::void_t<decltype(T{{Args{}}...})>`。
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
最后我们需要从结构体中取出每一个元素。此时*C++17*引入的结构化绑定会很有用，不过这里需要一些硬编码。
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
有点丑，不过挺有用。

## 未来计划
- 支持更多STL容器
- 支持json, xml
- 提升效率

## 需要的帮助
- 如果你懂如何提高序列化的效率，请联系我。