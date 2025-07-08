#include <iostream>
#include "../include/otas_serializer.h"

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

/*
g++ example.cpp -o example.exe -std=c++17
*/