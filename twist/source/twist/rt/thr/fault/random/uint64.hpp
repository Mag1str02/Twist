#pragma once

#include <twist/rt/thr/random/number.hpp>

namespace twist::rt::thr::fault {

inline uint64_t RandomUInt64() {
  return random::UInt64();
}

}  // namespace twist::rt::thr::fault
