# otas_serialization

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![Generic badge](https://img.shields.io/badge/C++-20-blue.svg)](https://shields.io/)

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

### 4.对struct的约束
进行序列化的struct需满足以下条件：
1. 无构造函数，或只有默认构造函数
2. 无private成员
3. 不包含未支持的STL容器
4. 不是派生类

### 5.对buffer的约束
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

## 性能测试
在性能测试中，otas_serialization相比其他序列工具展现出较为明显的性能提升。使用`yalantinglibs`给出的用例，进行1000000万次序列化，耗时如下:

| | rect | person | monster |
| otas_serialization | 24956 us | 68142 us | 260835 us |
| yalantinglibs | 54036 us | 104073 us | 290281 us | 