# otas_serialization

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![Generic badge](https://img.shields.io/badge/C++-20-blue.svg)](https://shields.io/)

[设计文档](https://github.com/maoliangcd/otas_serialization/blob/main/doc/design.md)

[English readme](https://github.com/maoliangcd/otas_serialization/blob/main/doc/readme_english.md)

这是一个只包含头文件的序列化库。不需要额外的配置文件和宏定义。

本项目参考自[yalantinglibs](https://github.com/alibaba/yalantinglibs)

## 快速开始:
### 1.示例
```cpp
#include <iostream>
#include <string>
#include "otas/otas_serializer.h"

struct Leaf {
    double x;
};
struct Otas {
    int a;
    Leaf b;
};

int main() {
    Otas otas1{1, {2.0}};
    auto s = otas_serializer::serialize(otas1);
    auto otas2 = otas_serializer::deserialize<Otas>(s);
    std::cout << otas2.a << " " << otas2.b.x << std::endl;
    system("pause");
    return 0;
}
```
没错，你不需要添加任何宏定义，不需要编写配置文件。只需要在代码中调用给定的接口。

本项目支持json的序列化与反序列化：
```cpp
#include <iostream>
#include <string>
#include "otas/otas_json.h"

struct Node {
    std::vector<int> a;
    std::vector<long> b;
    std::vector<char> c;
    std::vector<double> d;
};

int main() {
    Node node0{{1, 2, 3, 4}, {-1, -2, -3, -4}, {'x', 'y'}, {0.1, 1.1, 32.1}};
    auto s = serialize_json(node0);
    auto node1 = deserialize_json<Node>(s);
    system("pause");
    return 0;
}
```

序列化后的json字符串：
```json
{"a":[1,2,3,4],"b":[-1,-2,-3,-4],"c":["x","y"],"d":[0.100000,1.100000,32.100000]}
```

### 2.接口
```cpp
template <class Buffer = std::string, class T>
auto serialize(const T &obj)
```

| 参数 | 类型| 说明 |
| :--- | :--- | :--- |
| obj | 入参 | 序列化的对象|

返回值：
存储序列化字节流的buffer，默认为std::string

```cpp
template <class T, class Buffer>
auto deserialize(const Buffer &buffer)
```
| 参数 | 类型| 说明 |
| :--- | :--- | :--- |
| buffer | 入参 | 存储序列化字节流的buffer |

返回值：
反序列化获得的对象

```cpp
template <class T>
auto serialize_json(const T &obj)
```
| 参数 | 类型| 说明 |
| :--- | :--- | :--- |
| obj | 入参 | 序列化的对象 |

返回值：
json字符串，类型为std::string

```cpp
template <class T> 
auto deserialize_json(const std::string &buffer)
```
| 参数 | 类型| 说明 |
| :--- | :--- | :--- |
| buffer | 入参 | json字符串 |

返回值：
反序列化的结果

### 3.部署
将`include`目录下的内容复制到你的项目

### 4.测试
运行`benchmark`下的`.bat`或`.sh`脚本

### 5.对struct的约束
进行序列化的struct需满足以下条件：
1. 无自定义构造函数
2. 无private成员
3. 不包含裸指针
4. 不包含未支持的STL容器
5. 不是派生类
6. 成员个数不超过32个（可自行配置上限）

### 6.对buffer的约束
接受序列化结果的buffer需满足以下约束：
1. 实现`data()`方法和`resize()`方法
2. `data()`指向的地址空间连续，可通过下标`[]`进行访问
以下是一个buffer的例子
```cpp
struct otas_buffer {
    char *data_{};
    unsigned int size_{};
    char *data() const {
        return data_;
    }
    void resize(int size) {
        delete[] data_;
        data_ = new char[size];
        size_ = size;
        return ;
    }
};
```
样例：
```cpp
using namespace otas_serializer;

Otas otas1{1, {2.0}};
auto s = serialize<otas_buffer>(otas1);
auto otas2 = deserialize<Otas>(s);
```

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

## 自定义容器
本项目支持某些自定义容器，需满足以下约束
### map类型
该类型需提供`key_type`和`mapped_type`类型成员，`size()`，`begin()`，`end()`，`insert()`方法，若同时定义了`emplace()`方法，则会在反序列化时调用`emplace`方法。
```cpp
template <class T>
concept map_container = requires(T container) {
    typename T::key_type;
    typename T::mapped_type;
    container.size();
    container.begin();
    container.end();
    container.insert({typename T::key_type{}, typename T::mapped_type{}});
};
```

### set类型
该类型需提供`value_type`和`key_type`类型成员，`size()`，`begin()`，`end()`和`insert()`方法，若同时定义了`emplace()`方法，则会在反序列化时调用`emplace`方法。
```cpp
template <class T>
concept set_container = requires(T container) {
    typename T::key_type;
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
    container.insert(typename T::value_type{});
};
```

### vector类型
该类型需提供`value_type`类型成员，`size()`，`begin()`，`end()`和`resize()`方法，并重载了`operator[]`运算符。
```cpp
template <class T>
concept vector_container = requires(T container, unsigned int n) {
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
    container.resize(n);
    { container[n] } -> std::same_as<typename T::value_type &>;
};
```

### list类型
该类型需提供`value_type`类型成员，`size()`，`begin()`，`end()`和`insert()`方法，若同时定义了`emplace()`方法，则会在反序列化时调用`emplace`方法。
```cpp
template <class T>
concept list_container = requires(T container) {
    typename T::value_type;
    container.size();
    container.begin();
    container.end();
    container.insert(typename T::value_type{});
};
```

### string类型
该类型需提供`value_type`类型成员，且`value_type`类型为字符类型。提供`begin()`，`end()`，`size()`，`length()`，`data()`和`resize()`方法，且`data()`方法提供的内存连续，并重载了`operator[]`运算符。
```cpp
template <class T>
concept string_container = requires(T container, unsigned int n) {
    requires char_container<typename T::value_type>;
    container.size();
    container.begin();
    container.end();
    container.length();
    container.data();
    container.resize(n);
    { container[n] } -> std::same_as<typename T::value_type &>;
};
```

## 性能测试
在性能测试中，otas_serialization相比其他序列工具展现出较为明显的性能提升。使用`struct_pack`给出的用例，进行1000000万次序列化，耗时如下:

| | rect | person | monster |
| :--- | :--- | :--- | :--- |
| otas_serialization | 24956 us | 68142 us | 260835 us |
| yalantinglibs | 54036 us | 104073 us | 290281 us |

`struct_pack`使用`std::vector<char>`作为默认buffer，比`otas_serialization`进行性能测试时使用的自定义容器慢，因此两者实际差距比此处展示要小。