# otas_serialization

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![Generic badge](https://img.shields.io/badge/C++-17-blue.svg)](https://shields.io/)

[设计文档](https://github.com/maoliangcd/otas_serialization/blob/main/doc/design.md)

[English readme](https://github.com/maoliangcd/otas_serialization/blob/main/doc/readme_english.md)

这是一个只包含头文件的序列化库。不需要额外的配置文件和宏定义。

## 快速开始:
### 1.示例
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
没错，你不需要添加任何宏定义，不需要编写配置文件。只需要使用在你的代码中调用`otas_serializer::serialize`和`otas_serializer::deserialize`。

### 2.部署
将`include`目录下的内容复制到你的项目

### 3.测试
运行`benchmark`下的`.bat`或`.sh`脚本


## 支持的类型
- 基本类型(int, float, double, char...)
- 聚合类型struct(没有构造函数，没有私有成员)
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
你或许听说过编程中的反射(reflection)。反射是**程序在运行时可以访问、检测和修改它本身状态或行为的一种能力**，一个例子是动态获取结构中每个成员的类型与名字。不幸的是，C++几乎不支持动态反射，也不怎么支持静态反射（据说C++26会全面支持，但是参照C++委员会的效率和各编译器适配进度，估计也是猴年马月）。

静态反射发生在编译时，动态反射发生在运行时。C++专注于效率，如果能把工作提前到编译期，编译器就能进行更多的优化，因此对于C++反射的研究主要集中在静态反射。经过众多大佬的开发，**C++17**提供的反射能力终于到勉强能用的程度，这也是本项目的基础。


### 2.反射与序列化
那么反射有什么用处？在项目中，我们经常需要进行序列化和反序列化，要么给每个struct实现serialize()和deserialize()方法，要么用template针对不同struct进行特化。这项工作相当枯燥，而且很容易出错，此时一个想法自然地出现：把struct的结构看成一棵树，无论多么复杂，都是由struct、容器和基本类型所构成的，如果能够递归遍历整个struct，最终一定会遍历到容器和基本类型，此时只需要实现容器和基本类型的序列化方法，就能完成整个struct的序列化。

新的问题出现，如何遍历一个struct。我们希望编写的代码能够适配所有struct，但我们并不知道某个struct的布局，不知道它有几个成员，每个成员是什么类型。就算知道了这些，我们也不知道如何把每个成员从struct取出来进行递归遍历。这正是反射所要解决的问题，坐稳，下面我们开始操作。


### 3.元模板编程
这里会介绍一些模板的前置芝士，如果你已经掌握，请跳过。

#### 3.1 可变参数与递归
要在编译期就获得一切，离不开模板。模板是在编译时根据类型生成对应代码的机制，是C++实现静态多态的方法。由于在编译期生成代码，比虚函数要快，因此某些要求极低延迟（像量化交易）的程序会用模板来替代虚函数。

需要注意，模板作用于编译期，能也只能在编译期展开。记住这一点，大部分奇怪的写法都是因为这个限制。

`C++11`中引入了**可变参数模板(variadic template)**，让模板和普通函数一样能传递可变参数。而解包可变参数可以采用递归的方法。
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

#### 3.2 模板特化
在编写一个函数模板或类模板后，某些类可能需要一些特殊的实现，此时就可以使用模板特化。特化可分为全特化和偏特化。全特化就是针对某种类提供了特殊的实现。
```cpp
template <class T>
T add(T &a, T &b) {
    return a + b;
}
template <>
auto add<int>(int &a, int &b) {
    return a + b;
}
```

但有时，我们可能无法指定具体的类型。比如，我们希望针对指针`T *`或容器`std::vector<T>`的模板提供特殊实现。这里只对参数进行了部分限制，并没有指定具体的类型，因此被称为偏特化。偏特化进一步限制模板参数，只有类模板支持偏特化，函数模板不支持偏特化，因为函数可以进行重载，也能达到同样的效果。

```cpp
template <class T>
struct print_helper {
    static void print(T &t) {
        std::cout << t << std::endl;
    }
}

template <class T>
struct print_helper<std::vector<T>> {
    static void print(std::vector<T> &t) {
        for (auto &item : t) {
            std::cout << t << std::endl;
        }
    }
}
```
模板匹配的优先级为 全特化 > 偏特化 > 未特化

#### 3.3 SFINAE
SFINAE，全称**替换失败而非错误(substitution failure is not an error)**。在编译期模板展开错误时，不会报错，而是把发生错误的模板给丢弃。对开发者来说，就是你只管写，剩下的交给编译器，不成功也不会导致编译失败。C++17引入`std::void_t<T>`增强这一机制。如果T是一个合法类型表达式，会返回`void`，否则就会丢弃掉这个模板。

```cpp
template <class T, class = void>
struct has_member_name : std::false_type{};

template <class T>
struct has_member_name<T, std::void_t<class T::name>> : std::true_type{};
```
在这个例子中，如果`T`类型存在`name`成员，`std::void_t<T>`推导成功，`has_member_name<T>`会匹配到下面的偏特化版本，继承`std::true_type`。否则触发SFINAE，丢弃下方的模板，匹配到上面的未特化版本，继承`std::false_type`。


#### 3.4 结构化绑定
结构化绑定是**C++17**引入的一种解包方法，主要用于从`std::tuple`、`std::pair`和结构体中取出每个元素。同时，它也支持引用语义。比如，解包一个struct可以这样写：
```cpp
struct node {
    int a_;
    double b_;
    char c_;
};
node obj;
auto [a, b, c] = obj;
auto &[ar, br, cr] = obj;
```


### 4.获取结构体成员数量
前置芝士讲解完毕，下面看第一个问题，如何在编译时获得一个结构体的成员数量。**C++11**引入初始化列表，该语法在实例化对象时，按照顺序将列表中的值赋给结构体中的变量，只有在列表的长度小于等于结构体成员个数时，初始化才有可能成功。

```cpp
class Example {
    int a;
    int b;
};

Example t1{1};
Example t2{1, 2};
Example t3{1, 2, 3};
```
在这个例子中，`Example`有两个变量，因此前两个结构体的初始化会成功，第三个会失败。只要能不断构造结构体，每次增加一个参数，构造失败时，参数个数减一就是结构体成员个数。这里可以用`3.1`中的可变参数和递归来实现。

但是编译器如何知道构造是否成功？好巧，我们在`3.2`中讲到的`std::void_t<T>`恰好有这个作用。看下面这个例子：`any_type`中定义了类型转换函数`operator T() const`，可以隐式转换为任何类型。`std::remove_cv_t`和`std::remove_reference_t`用于类型萃取，前者能去除类型的`const`和`violate`属性，后者能去除类型的引用属性。

`member_count_struct`将我们讲过的芝士串联在一起，回顾一下：可变参数模板，模板递归，模板偏特化，SFINAE。
```cpp
struct any_type {
    template <typename T>
    operator T() const;
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

接下来，我们需要从结构体中取出每一个元素。这里恰好能用到**3.4**的结构化绑定，我们已经能得到结构体成员的个数，只要根据个数进行绑定就可以。不过结构化绑定需要一些硬编码，这里可以通过宏这样写：
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
- 更多STL容器
- 支持json, xml
- 提升效率
- 安全性与可靠性

## 需要的帮助
- 如果你懂如何提高序列化的效率，请联系我。