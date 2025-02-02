#pragma once

#include <twist/rt/thr/fault/std_like/atomic.hpp>

#include <wheels/core/assert.hpp>

#include <bit>
#include <cstdint>
#include <type_traits>

namespace twist::rt::thr::fault {

namespace futex {

using FaultyAtomicUint64 = FaultyAtomic<uint64_t>;

enum AtomicUint64Half {
  Low = 0,
  High = 1,
};

template <AtomicUint64Half Half>
class FaultyAtomicUint64Ref {
 public:
  using AtomicValueType = uint64_t;

  explicit FaultyAtomicUint64Ref(FaultyAtomicUint64& atomic)
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

  uint32_t* GetLowHalfLoc() const noexcept {
    uint32_t* atomic_parts = (uint32_t*)atomic_;

    if constexpr (IsLittleEndian()) {
      return atomic_parts + 0;
    } else if constexpr (IsBigEndian()) {
      return atomic_parts + 1;
    } else {
      wheels::Panic("Unsupported byte order");
    }
  }

  uint32_t* GetHighHalfLoc() const noexcept {
    uint32_t* atomic_parts = (uint32_t*)atomic_;

    if constexpr (IsLittleEndian()) {
      return atomic_parts + 1;
    } else if constexpr (IsBigEndian()) {
      return atomic_parts + 0;
    } else {
      wheels::Panic("Unsupported byte order");
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
  FaultyAtomicUint64* atomic_;
};

}  // namespace futex

}  // namespace twist::rt::thr::fault
