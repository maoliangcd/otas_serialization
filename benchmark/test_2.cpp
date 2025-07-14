#include "otas_serializer.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct Leaf {
    int x;
};

struct Node {
    std::vector<int> a;
    std::vector<std::vector<int>> b;
};

int main() {
    Node node1{{1}, {{2}, {3}}};
    std::string s;
    otas_serializer::serialize(node1, s);
    Node node2{};
    otas_serializer::deserialize(s, node2);
    assert(node1.a.size() == node2.a.size());
    assert(node1.a[0] == node2.a[0]);
    assert(node1.b.size() == node2.b.size());
    assert(node1.b[0].size() == node2.b[0].size());
    assert(node1.b[1].size() == node2.b[1].size());
    assert(node1.b[0][0] == node2.b[0][0]);
    assert(node1.b[1][0] == node2.b[1][0]);
    std::cout << "test_2 passed" << std::endl;
    return 0;
}