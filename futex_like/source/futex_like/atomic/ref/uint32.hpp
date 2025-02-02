#pragma once

#include <atomic>
#include <cstdint>

namespace futex_like {

namespace atomic {

class AtomicUint32Ref {
 public:
  using AtomicValueType = uint32_t;

  explicit AtomicUint32Ref(std::atomic_uint32_t& atomic)
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
  std::atomic_uint32_t* atomic_;
};

}  // namespace atomic

}  // namespace futex_like
