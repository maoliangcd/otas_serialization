#include "otas_serializer.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct Vec {
    std::vector<int> a;
    std::vector<std::vector<int>> b;
};

void TEST_CASE1() {
    Vec vec1{{1}, {{2}, {3}}};
    std::string s;
    otas_serializer::serialize(vec1, s);
    Vec vec2{};
    otas_serializer::deserialize(s, vec2);
    assert(vec1.a.size() == vec2.a.size());
    assert(vec1.a[0] == vec2.a[0]);
    assert(vec1.b.size() == vec2.b.size());
    assert(vec1.b[0].size() == vec2.b[0].size());
    assert(vec1.b[1].size() == vec2.b[1].size());
    assert(vec1.b[0][0] == vec2.b[0][0]);
    assert(vec1.b[1][0] == vec2.b[1][0]);
}

int main() {
    TEST_CASE1();
    std::cout << "test_2 passed" << std::endl;
    return 0;
}