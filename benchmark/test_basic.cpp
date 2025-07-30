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

struct St {
    std::string s;
    std::wstring ws;
};

void TEST_CASE2() {
    St st1{"abcd", L"abcd"};
    auto s = otas_serializer::serialize<St>(st1);
    auto st2 = otas_serializer::deserialize<St>(s);
    assert(st1.s == st2.s);
    assert(st1.ws == st2.ws);
}

struct Vec {
    std::vector<int> vec;
    std::deque<double> deq;
};

void TEST_CASE3() {
    Vec vec1{{1, 2, 3}, {0.1, 0.2, 0.3}};
    auto s = otas_serializer::serialize<Vec>(vec1);
    auto vec2 = otas_serializer::deserialize<Vec>(s);
    assert(vec1.vec.size() == vec2.vec.size());
    for (int i = 0; i < vec1.vec.size(); i++) {
        assert(vec1.vec[i] == vec2.vec[i]);
    }
    assert(vec1.deq.size() == vec2.deq.size());
    for (int i = 0; i < vec1.deq.size(); i++) {
        assert(vec1.deq[i] == vec2.deq[i]);
    }
}

int main() {
    TEST_CASE1();
    TEST_CASE2();
    std::cout << "test_basic passed" << std::endl;
    return 0;
}