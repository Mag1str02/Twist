#pragma once

#include <atomic>
#include <bit>
#include <cstdint>
#include <cstdlib>

namespace futex_like {

namespace atomic {

enum class AtomicUint64Half {
  Low = 0,
  High = 1,
};

template <AtomicUint64Half Half>
class AtomicUint64Ref {
 public:
  using AtomicValueType = uint64_t;

  explicit AtomicUint64Ref(std::atomic_uint64_t& atomic)
      : atomic_(&atomic) {
  }

  uint64_t AtomicLoad(std::memory_order mo) const noexcept {
    return atomic_->load(mo);
  }

  static uint32_t GetFutexValue(uint64_t atomic_value) noexcept {
    if constexpr (LowHalf()) {
      return (uint32_t)GetLowHalfValue(atomic_value);
    } else {
      return (uint32_t)GetHighHalfValue(atomic_value);
    }
  }

  uint32_t* FutexLoc() const noexcept {
    if constexpr (LowHalf()) {
      return GetLowHalfLoc();
    } else {
      return GetHighHalfLoc();
    };
  }

 private:
  static uint64_t GetLowHalfValue(uint64_t value) noexcept {
    return value & 0xFFFFFFFF;
  }

  static uint64_t GetHighHalfValue(uint64_t value) noexcept {
    return (value >> 32) & 0xFFFFFFFF;
  }

  uint32_t* AtomicParts() const noexcept {
    return (uint32_t*)atomic_;
  }

  uint32_t* GetLowHalfLoc() const noexcept {
    uint32_t* atomic_parts = AtomicParts();

    if constexpr (IsLittleEndian()) {
      return atomic_parts + 0;
    } else if constexpr (IsBigEndian()) {
      return atomic_parts + 1;
    } else {
      std::abort();  // Not supported
    }
  }

  uint32_t* GetHighHalfLoc() const noexcept {
    uint32_t* atomic_parts = AtomicParts();

    if constexpr (IsLittleEndian()) {
      return atomic_parts + 1;
    } else if constexpr (IsBigEndian()) {
      return atomic_parts + 0;
    } else {
      std::abort();  // Not supported
    }
  }

  static constexpr bool LowHalf() {
    return Half == AtomicUint64Half::Low;
  }

  static constexpr bool IsLittleEndian() {
    return std::endian::native == std::endian::little;
  }

  static constexpr bool IsBigEndian() {
    return std::endian::native == std::endian::big;
  }

 private:
  std::atomic_uint64_t* atomic_;
};

using AtomicUint64LowHalfRef =
    AtomicUint64Ref<AtomicUint64Half::Low>;
using AtomicUint64HighHalfRef =
    AtomicUint64Ref<AtomicUint64Half::High>;


}  // namespace atomic

}  // namespace futex_like
