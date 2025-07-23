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

## 未来计划
- 支持json, xml