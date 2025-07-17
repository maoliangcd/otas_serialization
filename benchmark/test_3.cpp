#include "otas_serializer.h"

#include <cassert>
#include <iostream>
#include <string>

struct Arr {
    std::array<int, 100> arr;
};

void TEST_CASE1() {
    Arr a;
    for (int i = 0; i < 100; i++) {
        a.arr[i] = i;
    }
    auto s = otas_serializer::serialize<Arr>(a);
    auto b = otas_serializer::deserialize<Arr>(s);
    for (int i = 0; i < 100; i++) {
        assert(a.arr[i] == b.arr[i]);
    }
}

int main() {
    TEST_CASE1();
    std::cout << "test_3 passed" << std::endl;
    return 0;
}