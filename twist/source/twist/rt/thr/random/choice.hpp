#pragma once

#include "number.hpp"

#include <wheels/core/assert.hpp>

#include <cstdlib>

namespace twist::rt::thr {

namespace random {

// [0, alts)
inline size_t Choice(size_t alts) {
  WHEELS_ASSERT(alts > 0, "Positive number of alternatives expected");
  return Number() % alts;
}

// [lo, hi)
inline size_t Choice(size_t lo, size_t hi) {
  WHEELS_ASSERT(lo < hi, "Non-empty range expected");
  return lo + Choice(hi - lo);
}

// ~ Choice(2)
inline bool Either() {
  return Choice(2) == 0;
}

inline size_t Either(size_t alts) {
  return Choice(alts);
}

}  // namespace random

}  // namespace twist::rt::thr
