#pragma once

#include <twist/rt/sim/user/library/std_like/chrono.hpp>

namespace twist::rt::sim {

namespace user::library::std_like {

class DeadLine {
  using Clock = std_like::chrono::Clock;

 public:
  DeadLine(Clock::time_point tp)
      : tp_(tp) {
  }

  template <typename Dur>
  static DeadLine FromTimeout(Dur timeout) {
    return DeadLine{Clock::now() +
                    std::chrono::duration_cast<Clock::duration>(timeout)};
  }

  bool Expired() const {
    return Clock::now() > tp_;
  }

  Clock::duration Left() const {
    auto now = Clock::now();
    if (tp_ > now) {
      return tp_ - now;
    } else {
      return Clock::duration{0};
    }
  }

  std::chrono::milliseconds LeftMillis() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(Left()) + std::chrono::milliseconds(1);
  }

  Clock::time_point TimePoint() const {
    return tp_;
  }

 private:
  Clock::time_point tp_;
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
