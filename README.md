# otas_serializer

A head-only serializer. No need to add any define in your code.

An example.
```cpp
#include <iostream>
#include "otas_serializer.h"
using namespace otas_serializer;
struct Node {
    int a;
    double b;
    char c;
};
int main() {
    Node node0{1, 2.0, 'c'};
    std::string s;
    std::size_t offset{};
    serialize(node0, s);
    Node node1{};
    deserialize(s, node1);
    std::cout << node1.a << " " << node1.b << " " << node1.c << std::endl;
    system("pause");
    return 0;
}
```