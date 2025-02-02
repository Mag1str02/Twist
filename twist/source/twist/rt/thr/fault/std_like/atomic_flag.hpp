#pragma once

#include <twist/rt/thr/fault/adversary/inject_fault.hpp>

#include <atomic>

namespace twist::rt::thr::fault {

class FaultyAtomicFlag {
 public:
  using Impl = ::std::atomic_flag;

 public:
  bool DebugTest(std::memory_order mo = std::memory_order::seq_cst) const {
    return impl_.test(mo);
  }

  // NOLINTNEXTLINE
  void clear(std::memory_order mo = std::memory_order::seq_cst) noexcept {
    InjectFaultBefore();
    impl_.clear(mo);
    InjectFaultAfter();
  }

  // NOLINTNEXTLINE
  bool test_and_set(std::memory_order mo = std::memory_order::seq_cst) noexcept {
    InjectFaultBefore();
    bool old_state = impl_.test_and_set(mo);
    InjectFaultAfter();
    return old_state;
  }

  // NOLINTNEXTLINE
  bool test(std::memory_order mo = std::memory_order::seq_cst) const noexcept {
    InjectFaultBefore();
    bool state = impl_.test(mo);
    InjectFaultAfter();
    return state;
  }

#if defined(__TWIST_ATOMIC_WAIT__)

  // NOLINTNEXTLINE
  void wait(bool old, std::memory_order mo = std::memory_order::seq_cst) {
    InjectFaultBefore();
    impl_.wait(old, mo);
    InjectFaultAfter();
  }

  // NOLINTNEXTLINE
  void notify_one() {
    InjectFaultBefore();
    impl_.notify_one();
    InjectFaultAfter();
  }

  // NOLINTNEXTLINE
  void notify_all() {
    InjectFaultBefore();
    impl_.notify_all();
    InjectFaultAfter();
  }

#endif

 private:
  Impl impl_;
};

}  // namespace twist::rt::thr::fault
