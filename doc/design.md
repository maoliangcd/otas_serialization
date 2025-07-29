## 原理
### 1.关于反射
你或许听说过编程中的**反射(reflection)**。反射是**程序在运行时可以访问、检测和修改它本身状态或行为的一种能力**，一个例子是动态获取结构中每个成员的类型与名字。不幸的是，C++几乎不支持动态反射，也不怎么支持静态反射（据说C++26会全面支持，但按照C++委员会的效率和各编译器适配进度，估计也是猴年马月）。

静态反射发生在编译时，动态反射发生在运行时。对于C++反射的研究主要集中在静态反射，经过众多大佬的开发，**C++17**提供的反射能力终于到勉强能用的程度，这也是本项目的基础。

那么反射有什么用处？在项目中，我们经常需要进行序列化和反序列化，常见方案是给每个struct实现serialize()和deserialize()方法，或者用template针对不同struct进行特化。这项工作相当枯燥，而且很容易出错。事实上，大部分序列化代码都是简单遍历一遍struct的成员，再调用每个成员的序列化方法。

此时一个想法自然地出现：如果有办法能自动遍历整个struct，不就不需要手写序列化代码了吗？但怎么做，我们并不知道某个struct的布局，不知道它有几个成员，每个成员是什么类型。这正是反射所要解决的问题，坐稳，下面我们开始操作。

### 2. 前置芝士

#### 2.1 可变参数模板
C++11中引入了可变参数模板(variadic template)，让模板和普通函数一样能传递可变参数。而解包可变参数可以采用模板递归的方法。在这个例子中，print_args会打印所有传入的参数。

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
需要注意，模板作用于编译期，能也只能在编译期展开，大部分看起来很奇怪的写法都是因为这个限制。

#### 2.2 模板特化
在编写一个通用的函数模板或类模板后，某些类可能需要一些特殊的实现，此时就可以使用模板特化。特化可分为全特化和偏特化。全特化就是针对某种类提供了特殊的实现。
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

但有时，我们无法指定具体的类型。比如，我们希望针对指针`T *`或容器`std::vector<T>`的模板提供特殊实现。这里只对参数进行了部分限制，并没有指定具体的类型，因此被称为偏特化。偏特化进一步限制模板参数，只有类模板支持偏特化，函数模板不支持偏特化，因为函数可以进行重载，也能达到同样的效果。
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
模板匹配的优先级为 全特化 > 偏特化 > 未特化，这点很有用。

#### 2.3 SFINAE
**SFINAE**，全称**替换失败而非错误(substitution failure is not an error)**。在编译期模板替换失败时，不会报错，而是把造成失败的模板给丢弃。对开发者来说，就是你只管写，剩下的交给编译器，不成功也不会导致编译失败。**C++17**引入`std::void_t<T>`增强这一机制。如果T是一个合法类型表达式，会返回void，否则就会丢弃掉这个模板。

```cpp
template <class T, class = void>
struct has_member_name : std::false_type{};

template <class T>
struct has_member_name<T, std::void_t<class T::name>> : std::true_type{};
```
在这个例子中，如果`T`类型存在`name`成员，`std::void_t<T>`推导成功，`has_member_name<T>`会匹配到下面的偏特化版本，继承`std::true_type`。否则触发SFINAE，丢弃下方的模板，匹配到上面的未特化版本，继承`std::false_type`。


#### 2.4 结构化绑定
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
不过，结构化绑定必须在编写代码时就确定到底有几个成员。要绑定有n个成员的struct，只能手写n个变量。

#### 2.5 列表初始化
C++11引入初始化列表，在实例化对象时，可以按照顺序将列表中的值赋给结构体中的变量，只有在列表的长度小于等于结构体成员个数时，初始化才有可能成功。

```cpp
class Example {
    int a;
    int b;
};

Example t1{1};
Example t2{1, 2};
Example t3{1, 2, 3};
```
在这个例子中，Example有两个变量，因此前两个结构体的初始化会成功，第三个会失败。

### 3. 反射实现

到目前，我们已经凑齐了静态反射所需的所有知识，下面看如何将它们组合起来。首先，如何在编译时获取结构体成员个数。

根据**初始化列表**的定义，列表长度超过结构体成员个数时，初始化会失败。只要能不断尝试构造结构体，每次增加一个参数，构造失败时，参数个数减一就是结构体成员个数。通过**可变参数模板**，每次在末尾的可变参数上附加一个新的变量，再结合**模板递归**，就能实现这种尝试。

但是编译器如何知道构造是否成功？好巧，我们在中讲到的`std::void_t<T>`就有这个作用。此时，我们就能得到如下的实现：

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

`any_type`的作用是占位符，其中定义了**类型转换函数**`operator T() const`。由于这是个函数模板，因此可以匹配任何类型，`any_type`就能隐式转换为任何类型。在递归尝试的过程中，每次在参数列表的末尾增加一个参数，用`decltype`来获取T的类型，同时使用`std::void_t`来判断类型是否合法。由于下方的模板是偏特化版本，因此会优先匹配下方模板。当触发SFINAE时，才会去匹配上方的未特化版本，此时的size-1就是struct成员个数。

当然，这个方法也是有限制的，初始化列表和构造函数不兼容，如果struct有构造函数，得到的个数就会不准确。

下一步是从结构体中取出每个成员，这里可以通过结构化绑定来获得。

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
在模板中，n代表成员个数，每个n都需要手写一个对应的模板。结构化绑定只能通过硬编码实现，所以没有更好的方法，只能通过宏定义减少重复工作。这里写了8个，就代表结构体的成员个数不能超过8个，如果想要更多就需要继续手写。所以你可以看到yalantinglibs也只支持256个成员（不过这部分代码可以用脚本生成）。

此处存在缺点，结构化绑定无法绑定private成员，因此序列化的对象不能含有private成员。

模板中通过`std::tie`将结构化绑定的结果打包成了`std::tuple`，到这里就可以在上层遍历结构体的每个成员了。结构化绑定中存储的是引用，因此需要通过类型萃取去除引用属性。

```cpp
constexpr auto count = get_member_count<T>();
auto members = member_tuple_helper<T, count>::tuple_view(t);
[&]<std::size_t... index>(std::index_sequence<index...>) {
    ((visit<remove_cvref_t<std::tuple_element_t<index, decltype(members)>>>(std::get<index>(members))), ...);
} (std::make_index_sequence<count>{});
```

由于从`std::tuple`中获得元素时下标必须是常量，因此无法通过for循环来遍历，需要编译期递归。**C++17**支持**constexpr lambda**，即编译期的lambda函数。通过逗号语义实现可变参数的解包，`std::index_sequence`生成常量下标序列，就可以遍历每个成员。至此静态反射的部分结束，我们有了不需要一行代码就能遍历结构体的方法，下面只需要在visit方法里实现序列化即可。




#### 4. 序列化实现

上一节中，我们已经完成静态反射的部分，但工作并没有就此结束，序列化实现仍然是一个复杂的工程。首先，我们要选择一种buffer来存储序列化后的数据。`std::string`是一个不错的选择。

接下来，我们需要对每种基本类型进行特化。
```cpp
template <>
struct serialize_helper<int> {
    static auto serialize_template(const int &t, std::string &s) {
        s.append(reinterpret_cast<char *>(&t), sizeof(t));
        return ;
    }
};
```
由于基本类型很多，可以用宏定义来帮助简化代码：
```cpp
#define GENERATE_TEMPLATE(T) \
template <> \
struct serialize_helper<T> { \
    static auto serialize_template(const T &t, std::string &s) { \
        s.append(reinterpret_cast<char *>(&t), sizeof(t)); \
        return ; \
    } \
}

GENERATE_TEMPLATE(int);
GENERATE_TEMPLATE(double);
GENERATE_TEMPLATE(float);
...
```
但这非常不优雅，有没有更好的实现？


##### 可平凡复制
`C++11`中，引入了**平凡类型**这一概念，并且提供了`std::is_trivially_copyable<T>`来判断一个类型是否是可平凡复制的。如果是，则该类型可以通过`memcpy`或`memmove`直接进行复制。而显然，所有的基本类型都是可平凡复制的。
因此，可以直接在未特化版本里加上这样一段：
```cpp
if constexpr (std::is_trivially_copyable_v<T>) {
    s.append(reinterpret_cast<char *>(&t), sizeof(t));
}
```
`if constexpr`是模板元编程的利器，会在编译时就根据对应结果生成相关代码。