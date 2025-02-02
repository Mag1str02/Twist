#pragma once

#include <chrono>

namespace wheels {

template <typename Clock = std::chrono::steady_clock>
class StopWatch {
  using TimePoint = typename Clock::time_point;
  using Duration = typename Clock::duration;

 public:
  StopWatch() : start_(Now()) {
    static_assert(Clock::is_steady, "Steady clock required");
  }

  Duration Elapsed() const {
    return Now() - start_;
  }

  Duration Restart() {
    auto elapsed = Elapsed();
    start_ = Now();
    return elapsed;
  }

 private:
  static TimePoint Now() {
    return Clock::now();
  }

 private:
  TimePoint start_;
};

}  // namespace wheels
