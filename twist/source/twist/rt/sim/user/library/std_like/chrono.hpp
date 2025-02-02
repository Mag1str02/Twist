#pragma once

#include <twist/rt/sim/system/time.hpp>
#include <twist/rt/sim/user/syscall/now.hpp>

namespace twist::rt::sim {

namespace user::library::std_like::chrono {

class Clock {
 public:
  using rep = system::Time::rep;
  using period = system::Time::period;
  using duration = system::Time::duration;
  using time_point = system::Time::time_point;

  static constexpr bool is_steady = true;

 public:
  static time_point now() {  // NOLINT
    return syscall::Now();
  }
};

}  // namespace user::library::std_like::chrono

}  // namespace twist::rt::sim
