#include "otas/otas_json.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace otas_serializer;

struct Node {
    std::vector<int> a;
    std::vector<long> b;
    std::vector<char> c;
    std::vector<double> d;
};

void TEST_CASE1() {
    Node node0{{1, 2, 3, 4}, {-1, -2, -3, -4}, {'x', 'y'}, {0.1, 1.1, 32.1}};
    auto s = serialize_json(node0);
    std::cout << s << std::endl;
    Node node1 = deserialize_json<Node>(s);
    assert(node0.a.size() == node1.a.size());
    for (size_t i = 0; i < node0.a.size(); i++) {
        assert(node0.a[i] == node1.a[i]);
    }
    assert(node0.b.size() == node1.b.size());
    for (size_t i = 0; i < node0.b.size(); i++) {
        assert(node0.b[i] == node1.b[i]);
    }
    assert(node0.c.size() == node1.c.size());
    for (size_t i = 0; i < node0.c.size(); i++) {
        assert(node0.c[i] == node1.c[i]);
    }
    assert(node0.d.size() == node1.d.size());
    return ;
}

int main() {
    TEST_CASE1();
    std::cout << "test_json passed" << std::endl;
    return 0;
}