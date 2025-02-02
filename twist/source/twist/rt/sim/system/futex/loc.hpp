#pragma once

#include "atomic.hpp"
#include "key.hpp"

#include <cstdint>

#include <wheels/core/assert.hpp>

namespace twist::rt::sim {

namespace system {

struct FutexLoc {
  void* atomic;
  uint8_t size;
  FutexAtomicPart part;
  FutexAtomicType type;

  static FutexLoc AtomicUint32(void* atomic) {
    return FutexLoc{atomic, 4,
                    FutexAtomicPart::Whole,
                    FutexAtomicType::Uint32};
  }

  static FutexLoc AtomicUint64Half(void* atomic, bool low) {
    return FutexLoc{atomic, 8,
                    low ? FutexAtomicPart::LowHalf : FutexAtomicPart::HighHalf,
                    FutexAtomicType::Uint64Half};
  }

  static FutexLoc StdAtomic(void* atomic, uint8_t size) {
    return FutexLoc{atomic, size,
                    FutexAtomicPart::Whole,
                    FutexAtomicType::Std};
  }

  // Atomic word location
  void* Atomic() const {
    return atomic;
  }

  size_t AtomicSize() const {
    return size;
  }

  struct FutexKey FutexKey() const {
    return {(uintptr_t)atomic, part};
  }
};

}  // namespace system

}  // namespace twist::rt::sim