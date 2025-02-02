#pragma once

#include <twist/rt/sim/user/library/std_like/atomic.hpp>

#include <twist/rt/sim/user/syscall/futex.hpp>
#include <twist/rt/sim/user/syscall/now.hpp>

#include <wheels/core/assert.hpp>

#include <chrono>
#include <utility>

namespace twist::rt::sim {

namespace user::library::futex {

// Atomics

using AtomicUint32 = std_like::atomic<uint32_t>;
using AtomicUint64 = std_like::atomic<uint64_t>;

// AtomicRef

class AtomicUint32Ref {
 public:
  using AtomicValueType = uint32_t;

  explicit AtomicUint32Ref(AtomicUint32& atomic)
      : atomic_(&atomic) {
  }

  uint32_t AtomicLoad(std::memory_order mo, wheels::SourceLocation call_site) const {
    return atomic_->load(mo, call_site);
  }

  static uint32_t GetFutexValue(uint32_t atomic_value) {
    return atomic_value;
  }

  system::FutexLoc FutexLoc() const {
    return system::FutexLoc::AtomicUint32(atomic_);
  }

 private:
  AtomicUint32* atomic_;
};

enum AtomicUint64Half {
  Low = 0,
  High = 1,
};

template <AtomicUint64Half Half>
class AtomicUint64Ref {
 public:
  using AtomicValueType = uint64_t;

  explicit AtomicUint64Ref(AtomicUint64& atomic)
      : atomic_(&atomic) {
  }

  uint64_t AtomicLoad(std::memory_order mo, wheels::SourceLocation call_site) const {
    return atomic_->load(mo, call_site);
  }

  static uint32_t GetFutexValue(uint64_t atomic_value) {
    if constexpr (Low()) {
      return (uint32_t)GetLowHalfValue(atomic_value);
    } else {
      return (uint32_t)GetHighHalfValue(atomic_value);
    }
  }

  system::FutexLoc FutexLoc() const {
    return system::FutexLoc::AtomicUint64Half(atomic_, Low());
  }

 private:
  static uint64_t GetLowHalfValue(uint64_t v) {
    return v & 0xFFFFFFFF;
  }

  static uint64_t GetHighHalfValue(uint64_t v) {
    return (v >> 32) & 0xFFFFFFFF;
  }

  static constexpr bool Low() {
    return Half == AtomicUint64Half::Low;
  }

  void* AtomicLoc() const {
    return atomic_;
  }

 private:
  AtomicUint64* atomic_;
};

// Atomic refs

using AtomicUint64LowHalfRef = AtomicUint64Ref<AtomicUint64Half::Low>;
using AtomicUint64HighHalfRef = AtomicUint64Ref<AtomicUint64Half::High>;

// Ref{Low, High}Half

inline auto RefLowHalf(AtomicUint64& atomic) {
  return AtomicUint64LowHalfRef{atomic};
}

inline auto RefHighHalf(AtomicUint64& atomic) {
  return AtomicUint64HighHalfRef{atomic};
}

// Wait

template <typename AtomicRef>
typename AtomicRef::AtomicValueType WaitImpl(AtomicRef atomic_ref, uint32_t old,
                                             std::memory_order mo,
                                             wheels::SourceLocation call_site) {
  typename AtomicRef::AtomicValueType atomic_curr_value;

  do {
    {
      system::WaiterContext waiter{system::FutexType::Futex, "futex::Wait",
                                   call_site};
      syscall::FutexWait(atomic_ref.FutexLoc(), old, &waiter);
    }

    atomic_curr_value = atomic_ref.AtomicLoad(mo, call_site);
  } while (AtomicRef::GetFutexValue(atomic_curr_value) == old);

  WHEELS_ASSERT(AtomicRef::GetFutexValue(atomic_curr_value) != old, "Exiting futex::Wait with old value");

  return atomic_curr_value;
}

inline auto Wait(AtomicUint32& atomic, uint32_t old,
          std::memory_order mo = std::memory_order::seq_cst,
          wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  return WaitImpl(AtomicUint32Ref{atomic}, old, mo, call_site);
}

template <AtomicUint64Half Half>
auto Wait(
    AtomicUint64Ref<Half> atomic_ref, uint32_t old,
          std::memory_order mo = std::memory_order::seq_cst,
          wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  return WaitImpl(atomic_ref, old, mo, call_site);
}

// WaitTimed

template <typename AtomicRef>
std::pair<bool, typename AtomicRef::AtomicValueType> WaitTimedImpl(AtomicRef atomic_ref, uint32_t old,
               std::chrono::milliseconds timeout,
               std::memory_order mo,
               wheels::SourceLocation call_site) {
  system::Time::Instant deadline = syscall::Now() + timeout;

  typename AtomicRef::AtomicValueType atomic_curr_value;

  do {
    if (syscall::Now() >= deadline) {
      return {false, 0};  // Arbitrary value
    }
    // Ignore errors

    system::WaiterContext waiter{system::FutexType::Futex, "futex::WaitTimed",
                                 call_site};
    syscall::FutexWaitTimed(atomic_ref.FutexLoc(), old, deadline, &waiter);

    atomic_curr_value = atomic_ref.AtomicLoad(mo, call_site);

  } while (AtomicRef::GetFutexValue(atomic_curr_value) == old);

  WHEELS_ASSERT(AtomicRef::GetFutexValue(atomic_curr_value) != old, "Exiting futex::Wait with old value");

  return {true, atomic_curr_value};
}

inline auto WaitTimed(AtomicUint32& atomic, uint32_t old,
               std::chrono::milliseconds timeout,
               std::memory_order mo = std::memory_order::seq_cst,
               wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  return WaitTimedImpl(AtomicUint32Ref{atomic}, old, timeout, mo, call_site);
}

template <AtomicUint64Half Half>
auto WaitTimed(
    AtomicUint64Ref<Half> atomic_ref, uint32_t old,
               std::chrono::milliseconds timeout,
               std::memory_order mo = std::memory_order::seq_cst,
               wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  return WaitTimedImpl(atomic_ref, old, timeout, mo, call_site);
}

// WakeKey

struct WakeKey {
  system::FutexLoc loc;
};

// PrepareWake

template <typename AtomicRef>
WakeKey PrepareWakeImpl(AtomicRef atomic_ref) {
  return {atomic_ref.FutexLoc()};
}

inline WakeKey PrepareWake(AtomicUint32& atomic) {
  return PrepareWakeImpl(AtomicUint32Ref{atomic});
}

template <AtomicUint64Half Half>
WakeKey PrepareWake(AtomicUint64Ref<Half> atomic_ref) {
  return PrepareWakeImpl(atomic_ref);
}

// Wake

void WakeOne(WakeKey key, wheels::SourceLocation call_site = wheels::SourceLocation::Current());
void WakeAll(WakeKey key, wheels::SourceLocation call_site = wheels::SourceLocation::Current());

}  // namespace user::library::futex

}  // namespace twist::rt::sim
