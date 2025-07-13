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
- `std::wstring`
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
- `std::tuple`
- `std::variant`
- `others(todo)`

## 原理
### 1.关于反射
你或许听说过编程中的反射(reflection)。反射是*程序在运行时可以访问、检测和修改它本身状态或行为的一种能力*，一个例子是动态获取结构中每个成员的类型与名字。不幸的是，C++几乎不支持动态反射，也不怎么支持静态反射（据说C++26会全面支持，但是参照C++委员会的效率和各编译器适配进度，估计也是猴年马月）。

动态反射发生在运行时，静态反射发生在编译时。C++专注于效率，如果能把工作提前到编译期，编译器就能进行更多的优化，因此对于C++反射的研究主要集中在静态反射。经过众多大佬的开发，*C++17*提供的反射能力终于到勉强能用的程度，这也是本项目的基础。


### 2.反射与序列化
那么反射有什么用处？在项目中，我们经常需要进行序列化和反序列化，要么给每个struct实现serialize()和deserialize()方法，要么用template针对不同struct进行特化。这项工作相当枯燥，而且很容易出错，此时一个想法自然地出现：把struct的结构看成一棵树，无论多么复杂，都是由struct、容器和基本类型所构成的，如果能够通过程序递归遍历整个struct，最终一定会遍历到容器和基本类型，此时只需要实现容器和基本类型的序列化方法，就能完成整个struct的序列化。

新的问题出现，如何遍历一个struct。我们希望编写的代码能够适配所有struct，但我们并不知道某个struct的布局，不知道它有几个成员，每个成员是什么类型。就算知道了这些，我们也不知道如何把每个成员从取出来进行递归遍历。这正是反射所要解决的问题，坐稳，下面我们开始操作。


### 3.元模板编程
这里会介绍一些模板的前置芝士，如果你已经掌握，请跳过。

要在编译期就获得一切，离不开模板。模板是在编译时根据类型生成对应代码的机制，是C++实现静态多态的方法。由于在编译期生成代码，比虚函数的动态多态要快，因此某些要求极低延迟（像量化交易）的程序会用模板来替代虚函数。

需要注意，模板作用于编译期，能也只能在编译期展开。记住这一点，大部分奇怪的写法都是因为这个限制。

#### 3.1 可变参数与递归
`C++11`中引入了可变参数模板(variadic template)，让模板和普通函数一样能传递可变参数。而要解包可变参数，可以采用递归的方法。
```cpp
template <class T> 
void print_args(T &t) {
    std::cout << t << std::endl;
}

template <class T, class ...Args>
void print_args(T &t, Args &...args) {
    std::cout << t << std::endl;
    print_args(args...);
}
```
在这个例子中，`print_args`会打印所有传入的参数。

#### 3.2 SFINAE
SFINAE，全称*替换失败而非错误(substitution failure is not an error)*。在编译期模板展开错误时，不会报错，而是把发生错误的模板给丢弃。对开发者来说，就是你只管写，剩下的交给编译器，不成功也不会导致编译失败。C++17引入`std::void_t`增强这一机制。

```cpp
template <class T, class = void>
struct node {
    
};
```

### 3.获取结构体成员数量
首先要解决的是如何在编译时获得一个结构体的成员数量。此时可以使用*C++11*引入的初始化列表。尝试用一个变量，两个变量，三个变量···来构造一个结构体。如果变量的数量小于等于结构体成员数量，构造函数就会成功，否则就会失败。这里有一个简单的例子：

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

这样就实现了一个能在编译器获取


## 未来计划
- 支持更多STL容器
- 支持json, xml
- 提升效率

## 需要的帮助
- 如果你懂如何提高序列化的效率，请联系我。