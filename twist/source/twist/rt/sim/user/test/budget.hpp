#pragma once

#include <twist/rt/sim/user/scheduler/interrupt.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/source_location.hpp>

namespace twist::rt::sim {

namespace user::test {

class Budget {
 public:
  explicit Budget(size_t count)
    : left_(count) {
  }

  bool Withdraw(size_t count, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    if (count > left_) {
      return false;
    }

    scheduler::Interrupt(call_site);

    if (left_ >= count) {
      left_ -= count;
      return true;
    } else {
      return false;
    }
  }

 private:
  size_t left_;
};

}  // namespace user::test

}  // namespace twist::rt::sim
