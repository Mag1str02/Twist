#pragma once

#include <twist/rt/sim/user/syscall/sync.hpp>
#include <twist/rt/sim/user/syscall/futex.hpp>
#include <twist/rt/sim/user/scheduler/preemption.hpp>
#include <twist/rt/sim/user/assist/sharable.hpp>

#include <wheels/core/compiler.hpp>

#include <atomic>
#include <utility>

namespace twist::rt::sim {

namespace user::library::std_like {

class atomic_flag {
  using State = uint32_t;

 public:
  atomic_flag(wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) {
    Init(source_loc);
  }

  // Non-copyable
  atomic_flag(const atomic_flag&) = delete;
  atomic_flag& operator=(const atomic_flag&) = delete;

  // Non-movable
  atomic_flag(atomic_flag&&) = delete;
  atomic_flag& operator=(atomic_flag&&) = delete;

  ~atomic_flag() {
    Destroy();
  }

  // NOLINTNEXTLINE
  void clear(std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    system::sync::Action clear{this, Size(), system::sync::ActionType::AtomicStore, 0, mo,
                               ValueType(), "atomic_flag::clear", call_site};
    syscall::Sync(&clear);

    StoreDebug(0);
  }

  // NOLINTNEXTLINE
  bool test_and_set(std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    system::sync::Action test{this, Size(), system::sync::ActionType::AtomicRmwLoad, 0, mo,
                              ValueType(), "atomic_flag::test_and_set", call_site};
    uint64_t r = syscall::Sync(&test);

    {
      scheduler::PreemptionGuard g;
      system::sync::Action set{this, Size(), system::sync::ActionType::AtomicRmwCommit, 1, mo,
                               ValueType(), "atomic_flag::test_and_set", call_site};
      syscall::Sync(&set);

      StoreDebug(1);
    }

    return (r == 1);
  }

  // NOLINTNEXTLINE
  bool test(std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) const noexcept {
    system::sync::Action test{(void*)this, Size(), system::sync::ActionType::AtomicLoad, 0, mo,
                              ValueType(), "atomic_flag::test", call_site};
    uint64_t r = syscall::Sync(&test);
    return (r == 1);
  }

  bool DebugTest(std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) const noexcept {
    scheduler::PreemptionGuard g;

    // Do not trace
    system::sync::Action test{(void*)this, Size(), system::sync::ActionType::AtomicDebugLoad, 0, mo,
                              ValueType(), "atomic_flag::DebugTest", call_site};
    uint64_t r = syscall::Sync(&test);
    return r == 1;
  }

#if defined(__TWIST_ATOMIC_WAIT__)

  // NOLINTNEXTLINE
  void wait(bool old, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    system::WaiterContext waiter{system::FutexType::Atomic, "atomic_flag::wait", call_site};
    while (test(mo) == old) {
      syscall::FutexWait(StdAtomicFlagFutexLoc(), (old ? 1 : 0), &waiter);
    }
  }

  // NOLINTNEXTLINE
  void notify_one(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    // TODO: Interrupt

    system::WakerContext ctx{"atomic_flag::notify_one", call_site};
    syscall::FutexWake(StdAtomicFlagFutexLoc(), 1, &ctx);
  }

  // NOLINTNEXTLINE
  void notify_all(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    // TODO: Interrupt

    system::WakerContext ctx{"atomic_flag::notify_all", call_site};
    syscall::FutexWake(StdAtomicFlagFutexLoc(), 0, &ctx);  // 0 - all
  }

#endif

 private:
  void Init(wheels::SourceLocation source_loc) {
    scheduler::PreemptionGuard g;  // ???
    system::sync::Action init{this, Size(), system::sync::ActionType::AtomicInit, 0, std::memory_order::relaxed /* ignored */,
                              ValueType(), "atomic_flag::atomic_flag", source_loc};
    syscall::Sync(&init);

    StoreDebug(0);
  }

  void Destroy() {
    scheduler::PreemptionGuard g;
    system::sync::Action destroy{this, Size(), system::sync::ActionType::AtomicDestroy, 0, std::memory_order::relaxed /* ignored */,
                                 ValueType(), "atomic_flag::~atomic_flag", wheels::SourceLocation::Current()};
    syscall::Sync(&destroy);
  }

  static system::sync::ValueType ValueType() {
    return system::sync::ValueType::Bool;
  }

#if defined(__TWIST_ATOMIC_WAIT__)

  system::FutexLoc StdAtomicFlagFutexLoc() {
    return system::FutexLoc::StdAtomic(this, 4);
  }

#endif

 private:
  void StoreDebug(bool v) {
    debug_value_ = v;
  }

  static constexpr uint8_t Size() {
    return 4;  // ???
  }

 private:
  [[maybe_unused]] bool debug_value_;  // Write-only
};

}  // namespace user::library::std_like

namespace user::assist {

template <>
struct Sharable<library::std_like::atomic_flag> {
  static const bool kStatus = false;
};

}  // namespace user::assist

}  // namespace twist::rt::sim
