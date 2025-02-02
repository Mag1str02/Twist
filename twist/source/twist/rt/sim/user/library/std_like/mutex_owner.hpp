#pragma once

#include <twist/rt/sim/user/syscall/id.hpp>

// #include <wheels/core/assert.hpp>
#include <twist/rt/sim/user/safety/assert.hpp>

namespace twist::rt::sim {

namespace user {

class MutexOwner {
 public:
  void Lock() {
    id_ = syscall::GetId();
  }

  bool Own() const {
    return id_ == syscall::GetId();
  }

  system::ThreadId Id() const {
    return id_;
  }

  void Unlock() {
    ___TWIST_SIM_USER_VERIFY(
        Own(),
        "The mutex must be locked by the current thread of execution");
    Reset();
  }

 private:
  void Reset() {
    id_ = system::kImpossibleThreadId;
  }

 private:
  system::ThreadId id_ = system::kImpossibleThreadId;
};

}  // namespace user

}  // namespace twist::rt::sim
