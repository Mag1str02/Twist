#pragma once

#include <cstdint>

namespace twist::rt::thr {

namespace random {

uint64_t UInt64();

inline uint64_t Number() {
  return UInt64();
}

}  // namespace random

}  // namespace twist::rt::thr
