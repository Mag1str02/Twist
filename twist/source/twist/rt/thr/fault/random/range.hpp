#pragma once

#include <twist/rt/thr/fault/random/uint64.hpp>

namespace twist::rt::thr::fault {

// [0, max], assume max < numeric_limits<uint64_t>::max()
inline uint64_t RandomUInteger(uint64_t max) {
  return RandomUInt64() % (max + 1);
}

// [lo, hi]
inline uint64_t RandomUInteger(uint64_t lo, uint64_t hi) {
  return lo + RandomUInteger(hi - lo);
}

}  // namespace twist::rt::thr::fault
