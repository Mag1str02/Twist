#pragma once

#include <twist/rt/thr/fault/adversary/inject_fault.hpp>
#include <twist/rt/thr/fault/random/range.hpp>

#include <mutex>

namespace twist::rt::thr {
namespace fault {

class FaultyMutex {
 public:
  FaultyMutex() {
    AccessAdversary();
  }

  void lock() {  // NOLINT
    InjectFaultBefore();
    impl_.lock();
    InjectFaultAfter();
  }

  bool try_lock() {  // NOLINT
    if (SpuriousFailure()) {
      return false;
    }

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

 private:
  static bool SpuriousFailure() {
    return RandomUInteger(11) == 0;
  }

 private:
  ::std::mutex impl_;
};

}  // namespace fault
}  // namespace twist::rt::thr
