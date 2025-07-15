#include <iostream>
#include <chrono>
#include "otas_serializer.h"

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
    std::forward_list<char> k;
};

int main() {
    Node node0{1, 2.0, '3', {1, 2, 3, 4, 5, 6, 7, 8},
        {{0.1, 0.2}, {0.3, 0.4, 0.5}, {0.6, 0.7, 0,8, 0.9}}, "hello world!", {{1, 2}}, 
        {{114514}, {1919810}}, {998244353}, {1.1, 2.2},{'a', 'b'}};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++) {
        std::string s;
        otas_serializer::serialize(node0, s);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_1 time: " << duration.count() << " us" << std::endl;
    return 0;
}
