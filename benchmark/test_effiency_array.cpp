#include <iostream>
#include <chrono>
#include "otas_serializer.h"

struct Node {
    std::array<int, 1000> a;
};

int main() {
    Node node0{};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++) {
        auto s = otas_serializer::serialize<Node>(node0);
        delete[] s.ptr;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_array time: " << duration.count() << " us" << std::endl;
    return 0;
}
