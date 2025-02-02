#pragma once

#include "atomic.hpp"

#include <cstdint>
#include <tuple>

namespace twist::rt::sim {

namespace system {

struct FutexKey {
  uintptr_t atomic;
  FutexAtomicPart part;

  bool operator==(FutexKey that) const {
    return (atomic == that.atomic) && (part == that.part);
  }

  bool operator!=(FutexKey that) const {
    return !(*this == that);
  }

  bool operator<(FutexKey that) const {
    return std::tie(atomic, part) < std::tie(that.atomic, that.part);
  }
};

}  // namespace system

}  // namespace twist::rt::sim
