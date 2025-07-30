#include <iostream>
#include <chrono>
#include "otas_serializer.h"

struct rect {
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

int main() {
    rect rect0{1, 2, 3, 4};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        otas_serializer::otas_buffer s;
        otas_serializer::serialize(rect0, s);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_rect time: " << duration.count() << " us" << std::endl;
    return 0;
}
