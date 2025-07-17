#include <iostream>
#include <chrono>
#include "otas_serializer.h"

struct Vec {
    std::vector<int> d;
};

int main() {
    Vec vec{{1, 2, 3, 4, 5, 6, 7, 8}};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++) {
        auto s = otas_serializer::serialize<Vec>(vec);
        delete[] s.ptr;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_1 time: " << duration.count() << " us" << std::endl;
    return 0;
}
