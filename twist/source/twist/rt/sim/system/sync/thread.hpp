#pragma once

#include "clock.hpp"

namespace twist::rt::sim {

namespace system::sync {

struct ThreadContext {
  struct Clock {
    VectorClock current;
    VectorClock acquire;
    VectorClock release;
    Epoch epoch;
  } clock;

  void Init() {
    clock.current.Init();
    clock.acquire.Init();
    clock.release.Init();
    clock.epoch = 0;
  }
};

}  // namespace system::sync

}  // namespace twist::rt::sim
