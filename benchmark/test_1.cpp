#include "otas_serializer.h"

#include <cassert>
#include <iostream>
#include <string>

struct Node {
    int a;
    unsigned int b;
    long long c;
    unsigned long long d;
    float e;
    double f;
    char g;
};

int main() {
    Node node1{-1, 2, -3, 4, 5.5, 6.6, '7'};
    std::string s;
    otas_serializer::serialize(node1, s);
    Node node2{};
    otas_serializer::deserialize(s, node2);
    assert(node1.a == node2.a);
    assert(node1.b == node2.b);
    assert(node1.c == node2.c);
    assert(node1.d == node2.d);
    assert(node1.e == node2.e);
    assert(node1.f == node2.f);
    assert(node1.g == node2.g);
    std::cout << "test_1 passed" << std::endl;
    return 0;
}