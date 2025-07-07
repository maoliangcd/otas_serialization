#include <iostream>
#include "../include/otas_serializer.h"

using namespace otas_serializer;

struct Node {
    int a;
    int b;
};

int main() {
    Node node0{1, 2};
    std::string s;
    std::size_t offset{};
    serialize(node0, s);
    Node node1{};
    deserialize(s, node1);
    std::cout << node1.a << " " << node1.b << std::endl;
    system("pause");
    return 0;
}

/*
g++ example.cpp -o example.exe -std=c++17
*/