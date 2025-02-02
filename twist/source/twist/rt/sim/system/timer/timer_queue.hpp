#pragma once

#include "../time.hpp"
#include "timer_heap.hpp"

#include <wheels/core/assert.hpp>

#include <chrono>

namespace twist::rt::sim {

namespace system {

class TimerQueue {
 public:
  void Add(Timer* timer) {
    timer->meta.id = ++next_id_;  // Tie-breaker
    timers_.Insert(timer);
  }

  bool Remove(Timer* timer) {
    if (timers_.IsActive(timer)) {
      timers_.Remove(timer);
      return true;
    } else {
      return false;
    }
  }

  bool IsEmpty() const {
    return timers_.IsEmpty();
  }

  Timer* Poll(Time::Instant now) {
    if (timers_.NonEmpty() && (timers_.Min()->when <= now)) {
      return timers_.ExtractMin();
    }
    return nullptr;
  }

  Time::Instant NextDeadLine() const {
    return timers_.Min()->when;
  }

  void Reset() {
    WHEELS_ASSERT(timers_.IsEmpty(), "Non-empty timer queue");
    next_id_ = 0;
  }

 private:
  TimerHeap timers_;
  TimerId next_id_ = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
