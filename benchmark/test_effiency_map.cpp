#include <iostream>
#include <chrono>
#include "otas_serializer.h"

struct Node {
    std::map<int, std::string> a;
};

int main() {
    Node node0{{{1,"h"}}};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++) {
        auto s = otas_serializer::serialize<Node>(node0);
        delete[] s.ptr;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_map time: " << duration.count() << " us" << std::endl;
    return 0;
}
