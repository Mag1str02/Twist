#pragma once

#include <twist/rt/thr/fault/adversary/inject_fault.hpp>

#include <mutex>
#include <chrono>

namespace twist::rt::thr {
namespace fault {

class FaultyTimedMutex {
 public:
  FaultyTimedMutex() {
    AccessAdversary();
  }

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

  // Timed
  bool try_lock_for(std::chrono::milliseconds timeout) {  // NOLINT
    InjectFaultBefore();
    bool acquired = impl_.try_lock_for(timeout);
    InjectFaultAfter();
    return acquired;
  }

  bool try_lock_until(std::chrono::system_clock::time_point deadline) {  // NOLINT
    InjectFaultBefore();
    bool acquired = impl_.try_lock_until(deadline);
    InjectFaultAfter();
    return acquired;
  }

  void unlock() {  // NOLINT
  }

 private:
  ::std::timed_mutex impl_;
};

}  // namespace fault
}  // namespace twist::rt::thr
