# otas_serialization

A header-only serialization tool. No configuration file, no additional define.

## Quick start:
### 1.Example
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
Yes! You don't need to add any define or configuration file to your code! Just apply `otas_serializer::serialize` and `otas_serializer::deserialize`.

### 2.Install
Copy the header files in `include` to your project.

### 3.Test
Run `.bat` or `.sh` script in `benchmark`.


## Supported type
- basic types(int, float, double, char...)
- aggregate type(no consturctor function, no private members)
- most of the STL containers

Support these STL containers：
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

## Future plan:
- json and xml