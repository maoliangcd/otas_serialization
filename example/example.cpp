#include <iostream>
#include <cstdint>
#include "../include/otas_serializer.h"

using namespace otas_serializer;

struct Leaf {
    int x;
    bool operator<(const Leaf &leaf) const {
        return x > leaf.x;
    }
};

struct Node {
    int a;
    double b;
    char c;
    std::vector<int> d;
    std::vector<std::vector<float>> e;
    std::string f;
    std::map<int, int> g;
    std::set<Leaf> h;
    std::optional<int> i;
    std::pair<double, double> j;
    std::unique_ptr<int> k;
    std::forward_list<char> l;
    std::tuple<std::string, int> m;
    std::variant<int, char> n;
};

int main() {
    std::cout << get_member_count<Node>() << std::endl;
    Node node0{1, 2.0, '3', {1, 2}, {{0.1, 0.2}, {0.3, 0.4, 0.5}},
        "hello world!", {{1, 2}}, {{114514}, {1919810}}, {998244353}, {1.1, 2.2},{},{'a', 'b'}};
    auto ptr = new int;
    *ptr = 2025;
    node0.k.reset(ptr);
    std::get<0>(node0.m) = "noname";
    std::get<1>(node0.m) = -1;
    node0.n = 'y';
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
    for (auto &item : node1.h) {
        std::cout << item.x << std::endl;
    }

    std::array<int, 2> arr{333, 444};
    std::string s2;
    serialize(arr, s2);
    std::array<int, 2> arr2;
    deserialize(s2, arr2);
    for (const auto &item : arr2) {
        std::cout << item << std::endl;
    }
    if (node1.i) {
        std::cout << node1.i.value() << std::endl;
    }
    std::cout << node1.j.first << " " << node1.j.second << std::endl;
    std::cout << *(node1.k) << std::endl;
    for (const auto &item : node1.l) {
        std::cout << item << std::endl;
    }
    std::cout << std::get<0>(node1.m) << " " << std::get<1>(node1.m) << std::endl;
    std::cout << std::get<char>(node1.n) << std::endl;
    system("pause");
    return 0;
}

/*
g++ example.cpp -o example.exe -std=c++17
*/