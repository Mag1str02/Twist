#pragma once

#include <twist/rt/sim/system/time.hpp>

#include <cstdint>

namespace twist::rt::sim {

namespace system {

using TimerId = uint64_t;

struct TimerMeta {
  uintmax_t loc;
  TimerId id;
};

struct Timer {
  Time::Instant when;

  // Set by timer scheduler
  TimerMeta meta;

  virtual void Alarm() noexcept = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
