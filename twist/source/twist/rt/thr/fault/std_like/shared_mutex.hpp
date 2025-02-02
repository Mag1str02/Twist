#pragma once

#include <twist/rt/thr/fault/adversary/inject_fault.hpp>

#include <shared_mutex>

namespace twist::rt::thr {
namespace fault {

class FaultySharedMutex {
 public:
  FaultySharedMutex() {
    AccessAdversary();
  }

  // Writer

  void lock() {  // NOLINT
    InjectFaultBefore();
    impl_.lock();
    InjectFaultAfter();
  }

  bool try_lock() {  // NOLINT
    InjectFaultBefore();
    bool acquired = impl_.try_lock();
    InjectFaultAfter();
    return acquired;
  }

  void unlock() {  // NOLINT
    InjectFaultBefore();
    impl_.unlock();
    InjectFaultAfter();
  }

  // Reader

  void lock_shared() {  // NOLINT
    InjectFaultBefore();
    impl_.lock_shared();
    InjectFaultAfter();
  }

  bool try_lock_shared() {  // NOLINT
    InjectFaultBefore();
    bool acquired = impl_.try_lock_shared();
    InjectFaultAfter();
    return acquired;
  }

  void unlock_shared() {  // NOLINT
    InjectFaultBefore();
    impl_.unlock_shared();
    InjectFaultAfter();
  }

 private:
  ::std::shared_mutex impl_;
};

}  // namespace fault
}  // namespace twist::rt::thr
