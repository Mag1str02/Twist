#pragma once

#include <cstdint>

namespace twist::rt::sim {

namespace system {

enum FutexAtomicType : uint8_t {
  Uint32,
  Uint64Half,
  Std,
};

enum FutexAtomicPart : uint8_t {
  Whole,
  LowHalf,
  HighHalf
};

}  // namespace system

}  // namespace twist::rt::sim
