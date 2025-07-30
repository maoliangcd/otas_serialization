#include <iostream>
#include <chrono>
#include "otas_serializer.h"

struct Vec3 {
  float x;
  float y;
  float z;
};

struct Weapon {
  std::string name;
  int16_t damage;
};

struct Monster {
  Vec3 pos;
  int16_t mana;
  int16_t hp;
  std::string name;
  std::vector<uint8_t> inventory;
  std::vector<Weapon> weapons;
  Weapon equipped;
  std::vector<Vec3> path;
};

int main() {
    Monster monster0{{1, 2, 3}, 1, 1, "mon3tr", {1, 2, 3},
        {{"m1", 1}, {"m2", 2}, {"m3", 3}}, {"doctor", 0}, {{0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}}};
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++) {
        auto s = otas_serializer::serialize<otas_serializer::otas_buffer>(monster0);
        delete[] s.data_;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "test_effiecncy_monster time: " << duration.count() << " us" << std::endl;
    return 0;
}
