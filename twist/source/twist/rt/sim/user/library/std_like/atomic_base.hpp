#pragma once

#include "atomicable.hpp"

#include <twist/rt/sim/user/syscall/sync.hpp>
#include <twist/rt/sim/user/syscall/futex.hpp>

#include <twist/rt/sim/user/scheduler/preemption.hpp>
#include <twist/rt/sim/user/scheduler/spurious.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/source_location.hpp>

#include <cassert>
// std::memory_order
#include <atomic>
// std::memcmp
#include <cstring>
#include <type_traits>
// std::exchange
#include <utility>
#include <cstdint>

#include <wheels/core/compiler.hpp>

namespace twist::rt::sim {

namespace user::library::std_like {

template <typename T,
          bool Int = std::is_integral_v<T> && !std::is_same_v<T, bool>>
class AtomicBase;

// AtomicBase for non-integral types

template <typename T>
class AtomicBase<T, false> {
 public:
  using value_type = T;  // NOLINT

  AtomicBase(wheels::SourceLocation /*source_loc*/) {
    static_assert(Atomicable<T>::kStatus);
  }

  // Non-copyable
  AtomicBase(const AtomicBase<T>&) = delete;
  AtomicBase<T>& operator=(const AtomicBase<T>&) = delete;

  // Non-movable
  AtomicBase(AtomicBase<T>&&) = delete;
  AtomicBase<T>& operator=(AtomicBase<T>&&) = delete;

  ~AtomicBase() {
    // Explicit destroy required
  }

  bool is_lock_free() const noexcept {
    return true;
  }

  static constexpr bool is_always_lock_free = true;

  // NOLINTNEXTLINE
  T load(std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) const noexcept {
    system::sync::Action load{(void*)this, Size(), system::sync::ActionType::AtomicLoad, 0, mo,
                              ValueType(), "atomic::load", call_site};
    uint64_t r = syscall::Sync(&load);

    return ToValue(r);
  }

  T DebugLoad(std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) const noexcept {
    scheduler::PreemptionGuard g;

    system::sync::Action load{(void*)this, Size(), system::sync::ActionType::AtomicDebugLoad, 0, mo,
                              ValueType(), "atomic::DebugLoad", call_site};
    uint64_t r = syscall::Sync(&load);

    return ToValue(r);  // Do not trace
  }

  operator T() const noexcept {
    return load();
  }

  // NOLINTNEXTLINE
  void store(T new_value, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    system::sync::Action store{this, Size(), system::sync::ActionType::AtomicStore, ToRepr(new_value), mo,
                               ValueType(), "atomic::store", call_site};
    syscall::Sync(&store);

    StoreDebug(new_value);
  }

  // NOLINTNEXTLINE
  T exchange(T new_value, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    system::sync::Action load{this, Size(), system::sync::ActionType::AtomicRmwLoad, 0, mo,
                              ValueType(), "atomic::exchange", call_site};
    uint64_t r = syscall::Sync(&load);

    {
      scheduler::PreemptionGuard g;
      system::sync::Action store{this, Size(), system::sync::ActionType::AtomicRmwCommit, ToRepr(new_value), mo,
                                 ValueType(), "atomic::exchange", call_site};
      syscall::Sync(&store);
    }

    StoreDebug(new_value);

    return ToValue(r);
  }

  // NOLINTNEXTLINE
  bool compare_exchange_strong(
      T& expected, T desired,
      std::memory_order success,
      std::memory_order failure,
      wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    if (success == failure) {
      return CompareExchangeSimple(expected, desired, success, CasType::Strong, call_site);
    } else {
      return CompareExchangeWithFailureMemoryOrder(
          expected, desired, success, failure, CasType::Strong, call_site);
    }
  }

  // NOLINTNEXTLINE
  bool compare_exchange_strong(T& expected, T desired, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return CompareExchangeSimple(expected, desired, mo, CasType::Strong, call_site);
  }

  // NOLINTNEXTLINE
  bool compare_exchange_weak(
      T& expected, T desired,
      std::memory_order success,
      std::memory_order failure,
      wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    if (success == failure) {
      return CompareExchangeSimple(expected, desired, success, CasType::Weak, call_site);
    } else {
      return CompareExchangeWithFailureMemoryOrder(
          expected, desired, success, failure, CasType::Weak, call_site);
    }
  }

  // NOLINTNEXTLINE
  bool compare_exchange_weak(T& expected, T desired, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return CompareExchangeSimple(expected, desired, mo, CasType::Weak, call_site);
  }

  // wait / notify

#if defined(__TWIST_ATOMIC_WAIT__)

  // NOLINTNEXTLINE
  void wait(T old, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    system::WaiterContext waiter{system::FutexType::Atomic, "atomic::wait", call_site};
    while (BitwiseEqual(load(mo), old)) {
      syscall::FutexWait(AtomicWaitFutexLoc(), ToRepr(old), &waiter);
    }
  }

  // NOLINTNEXTLINE
  void notify_one(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    WHEELS_UNUSED(call_site);
    // TODO: Interrupt
    system::WakerContext ctx{"atomic::notify_one", call_site};
    syscall::FutexWake(AtomicWaitFutexLoc(), 1, &ctx);
  }

  // NOLINTNEXTLINE
  void notify_all(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    WHEELS_UNUSED(call_site);
    // TODO: Interrupt
    system::WakerContext ctx{"atomic::notify_all", call_site};
    syscall::FutexWake(AtomicWaitFutexLoc(), 0, &ctx);
  }

#endif

  system::FutexLoc FutexLoc() {
    if constexpr (std::is_same_v<T, uint32_t>) {
      return system::FutexLoc::AtomicUint32(this);
    } else {
      wheels::Panic("FutexLoc not supported");
    }
  }

 private:
  enum class CasType {
    Weak,
    Strong
  };

  static bool IsWeak(CasType type) {
    return type == CasType::Weak;
  }

  static const char* CompareExchangeOperationName(CasType type) {
    switch (type) {
      case CasType::Weak:
        return "atomic::compare_exchange_weak";
      case CasType::Strong:
        return "atomic::compare_exchange_strong";
      default:
        WHEELS_UNREACHABLE();
    }
  }

  static bool CompareExchangeSpuriousFailure(CasType type) {
    return IsWeak(type) && scheduler::SpuriousTryFailure();
  }

  bool CompareExchangeSimple(
      T& expected, T desired,
      std::memory_order mo,
      CasType type,
      wheels::SourceLocation call_site) {

    system::sync::Action load{this, Size(), system::sync::ActionType::AtomicRmwLoad, 0, mo,
                              ValueType(), CompareExchangeOperationName(type), call_site};
    uint64_t r = syscall::Sync(&load);

    T old_value = ToValue(r);

    scheduler::PreemptionGuard g;

    if (BitwiseEqual(expected, old_value) && !CompareExchangeSpuriousFailure(type)) {
      system::sync::Action store{this, Size(), system::sync::ActionType::AtomicRmwCommit, ToRepr(desired), mo,
                                 ValueType(), CompareExchangeOperationName(type), call_site};
      syscall::Sync(&store);

      StoreDebug(desired);

      return true;
    } else {
      expected = old_value;
      return false;
    }
  }

  static bool IsAtLeastAcquireMemoryOrder(std::memory_order mo) {
    switch (mo) {
      case std::memory_order::acquire:
      case std::memory_order::acq_rel:
      case std::memory_order::seq_cst:
        return true;
      default:
        return false;
    }
  }

  void CompareExchangeAcquireFence(const char* operation, wheels::SourceLocation call_site) noexcept {
    system::sync::Action fence{this, Size(), system::sync::ActionType::AtomicThreadFence, 0 /*ignored*/, std::memory_order::acquire,
                               system::sync::ValueType::None, operation, call_site};
    syscall::Sync(&fence);
  }

  bool CompareExchangeWithFailureMemoryOrder(
      T& expected, T desired,
      std::memory_order success,
      std::memory_order failure,
      CasType type,
      wheels::SourceLocation call_site) noexcept {
    system::sync::Action load{this, Size(), system::sync::ActionType::AtomicRmwLoad, 0, std::memory_order::relaxed,
                              ValueType(), CompareExchangeOperationName(type), call_site};
    uint64_t r = syscall::Sync(&load);

    T old_value = ToValue(r);

    scheduler::PreemptionGuard g;

    if (BitwiseEqual(expected, old_value) && !CompareExchangeSpuriousFailure(type)) {
      if (IsAtLeastAcquireMemoryOrder(success)) {
        CompareExchangeAcquireFence(CompareExchangeOperationName(type), call_site);
      }

      system::sync::Action store{this, Size(), system::sync::ActionType::AtomicRmwCommit, ToRepr(desired), success,
                                 ValueType(), CompareExchangeOperationName(type), call_site};
      syscall::Sync(&store);

      StoreDebug(desired);

      return true;
    } else {
      if (IsAtLeastAcquireMemoryOrder(failure)) {
        CompareExchangeAcquireFence(CompareExchangeOperationName(type), call_site);
      }

      expected = old_value;
      return false;
    }
  }

 protected:
  void Init(T value, wheels::SourceLocation source_loc) {
    {
      scheduler::PreemptionGuard g;  // ???

      system::sync::Action init{this, Size(), system::sync::ActionType::AtomicInit, ToRepr(value), std::memory_order::relaxed /* ignored */,
                                ValueType(), "atomic::atomic", source_loc};
      syscall::Sync(&init);
    }

    StoreDebug(value);
  }

  void Destroy() {
    scheduler::PreemptionGuard g;
    system::sync::Action destroy{this, Size(), system::sync::ActionType::AtomicDestroy, 0, std::memory_order::relaxed /* ignored */,
                                 ValueType(), "atomic::~atomic", wheels::SourceLocation::Current()};
    syscall::Sync(&destroy);
  }

  void StoreDebug(T v) {
    debug_value_ = v;
  }

  static uint64_t ToRepr(T v) {
    uint64_t r = 0;
    std::memcpy(&r, &v, sizeof(T));
    return r;
  }

  static T ToValue(uint64_t r) {
    T v;
    std::memcpy(&v, &r, sizeof(T));
    return v;
  }

  static bool BitwiseEqual(const T& lhs, const T& rhs) {
    return std::memcmp(&lhs, &rhs, sizeof(T)) == 0;
  }

  static system::sync::ValueType ValueType() {
    return system::sync::GetValueType<T>();
  }

  static constexpr uint8_t Size() {
    return sizeof(T);
  }

 private:
#if defined(__TWIST_ATOMIC_WAIT__)
  system::FutexLoc AtomicWaitFutexLoc() {
    return system::FutexLoc::StdAtomic(this, sizeof(T));
  }
#endif

 private:
  [[maybe_unused]] T debug_value_;  // Write-only
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
