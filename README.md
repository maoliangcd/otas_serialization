# otas_serializer

A header-only serializer. No need to add anything in your code. 

Support these STL containers:
- std::string
- std::vector
- std::map
- std::unordered_map
- std::multimap
- std::set
- std::unordered_set
- std::multiset
- std::queue
- std::deque
- std::list
- std::array

Q&A:
1.  Q: Support language?
    A: C++17 and above.
2.  Q: How does it work?
    A: This project uses static reflection of C++. By using template, it can generete serializer for each struct during compiling. So you no need to add code in your struct.
3.  Q: How to use?
    A: Here is an example.

An example.
```cpp
#include <iostream>
#include "otas_serializer.h"
using namespace otas_serializer;

struct Node {
    int a;
    double b;
    char c;
    std::vector<int> d;
    std::vector<std::vector<float>> e;
    std::string f;
    std::map<int, int> g;
};

int main() {
    Node node0{1, 2.0, '3', {1, 2}, {{0.1, 0.2}, {0.3, 0.4, 0.5}}, "hello world!", {{1, 2}}};
    std::string s;
    std::size_t offset{};
    serialize(node0, s);
    Node node1{};
    deserialize(s, node1);
    std::cout << node1.a << " " << node1.b << " " << node1.c << std::endl;
    for (auto &item : node1.d) {
        std::cout << item << std::endl;
    }
    for (auto &vec : node1.e) {
        for (auto &item : vec) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
    std::cout << node1.f << std::endl;
    for (auto &pair : node1.g) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    system("pause");
    return 0;
}
```