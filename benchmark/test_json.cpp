#include "otas/otas_json.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace otas_serializer;

struct Node {
    std::vector<int> a;
    std::vector<long> b;
    std::vector<char> c;
    std::map<int, double> d;
};

void TEST_CASE1() {
    Node node{{1, 2, 3, 4}, {-1, -2, -3, -4}, {'x', 'y'}, {{1, 0.1}, {2, 0.2}}};
    auto s = serialize_json(node);
    std::cout << s << std::endl;
}

int main() {
    TEST_CASE1();
    std::cout << "test_json passed" << std::endl;
    return 0;
}