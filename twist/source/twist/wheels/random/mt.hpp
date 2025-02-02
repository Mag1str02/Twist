#pragma once

#include <random>

namespace twist::random {

class MersenneTwister {
 public:
  explicit MersenneTwister(uint64_t seed)
      : twister_(seed) {
  }

  void Seed(uint64_t seed) {
    twister_.seed(seed);
  }

  uint64_t Next() {
    return twister_();
  }

 private:
  std::mt19937_64 twister_;
};

}  // namespace twist::random
