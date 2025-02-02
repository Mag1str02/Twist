#pragma once

#include <twist/rt/thr/fault/std_like/atomic.hpp>

namespace twist::rt::thr::fault {

namespace futex {

using FaultyAtomicUint32 = FaultyAtomic<uint32_t>;

class FaultyAtomicUint32Ref {
 public:
  using AtomicValueType = uint32_t;

  explicit FaultyAtomicUint32Ref(FaultyAtomicUint32& atomic)
      : atomic_(&atomic) {
  }

  uint32_t AtomicLoad(std::memory_order mo) const noexcept {
    return atomic_->load(mo);
  }

  static uint32_t GetFutexValue(uint32_t atomic_value) noexcept {
    return atomic_value;
  }

  uint32_t* FutexLoc() const noexcept {
    return (uint32_t*)atomic_;
  }

 private:
  FaultyAtomicUint32* atomic_;
};

}  // namespace futex

}  // namespace twist::rt::thr::fault
