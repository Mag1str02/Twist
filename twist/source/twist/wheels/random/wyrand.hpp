#pragma once

#include <cstdint>

namespace twist::random {

// https://github.com/lemire/testingRNG/blob/master/source/wyrand.h

class WyRand {
 public:
  explicit WyRand(uint64_t seed)
      : state_(seed) {
  }

  void Seed(uint64_t seed) {
    state_ = seed;
  }

  uint64_t Next() {
    return Next(&state_);
  }

 private:
  static uint64_t Next(uint64_t* s) {
    *s += UINT64_C(0xa0761d6478bd642f);
    __uint128_t t = (__uint128_t)*s * (*s ^ UINT64_C(0xe7037ed1a0b428db));
    return (t >> 64) ^ t;
  }

 private:
  uint64_t state_;
};

}  // namespace twist::random
