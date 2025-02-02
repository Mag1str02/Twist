#pragma once

#include <twist/rt/fault/random/uint64.hpp>

namespace twist::rt::thr::fault {

inline uint64_t RandomTower() {
  uint64_t bits = RandomUInt64();
  return __builtin_ctzl(bits);
}

}  // namespace twist::rt::thr::fault
