#pragma once

#include "ticker.hpp"
#include "params.hpp"

#include <wheels/core/noncopyable.hpp>
#include <wheels/core/assert.hpp>

#include <chrono>

namespace twist::rt::sim {

namespace system {

class Time : private wheels::NonCopyable {
 public:
  using duration = std::chrono::nanoseconds;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<Time>;

  using Instant = time_point;
  using Duration = duration;

 public:
  Time(Ticker& ticker, Params params)
      : ticker_(ticker),
        tick_(params.tick) {
  }

  void AdvanceTo(Instant future) {
    WHEELS_VERIFY(future >= Now(), "Time cannot move backward");

    Ticks f = ToTicks(future);
    Ticks n = NowTicks();
    WHEELS_VERIFY(f >= n, "Time is broken");

    ticker_.Tick(f - n);
  }

  void Reset() {
    ticker_.Reset();
  }

  Instant Now() const {
    return Instant{ToDur(NowTicks())};
  }

  Instant After(Duration delay) const {
    return Now() + delay;
  }

 private:
  Duration ToDur(Ticks count) const {
    return count * tick_;
  }

  Ticks ToTicks(Duration d) const {
    auto x = d.count();
    auto y = tick_.count();

    return (x / y) + (x % y != 0);
  }

  Ticks ToTicks(Instant i) const {
    return ToTicks(i.time_since_epoch());
  }

  Ticks NowTicks() const {
    return ticker_.Count();
  }

 private:
  Ticker& ticker_;
  Duration tick_;
};

}  // namespace system

};  // namespace twist::rt::sim
