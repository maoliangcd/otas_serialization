#include "otas_serializer.h"

#include <cassert>
#include <iostream>
#include <string>

#include "otas_check.h"

struct Node {
    int a;
    unsigned int b;
    long long c;
    unsigned long long d;
    float e;
    double f;
    char g;
};

struct Leaf {
    int a;
};

void TEST_CASE1() {
    Node node1{-1, 2, -3, 4, 5.5, 6.6, '7'};
    auto s = otas_serializer::serialize<Node>(node1);
    auto node2 = otas_serializer::deserialize<Node>(s);
    assert(node1.a == node2.a);
    assert(node1.b == node2.b);
    assert(node1.c == node2.c);
    assert(node1.d == node2.d);
    assert(node1.e == node2.e);
    assert(node1.f == node2.f);
    assert(node1.g == node2.g);
}

int main() {
    TEST_CASE1();
    std::cout << "test_1 passed" << std::endl;
    return 0;
}