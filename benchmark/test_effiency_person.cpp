#include <iostream>
#include <chrono>
#include "otas_serializer.h"

struct person {
  int64_t id;
  std::string name;
  int age;
  double salary;
};

int main() {
    person person1{998244353, "jiangly", 18, 1000000.0};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        auto s = otas_serializer::serialize<person>(person1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_person time: " << duration.count() << " us" << std::endl;
    return 0;
}
