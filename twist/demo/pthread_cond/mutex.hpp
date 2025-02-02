#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

// uint32_t
#include <cstdint>

class Mutex {
  enum State : uint32_t {
    Free = 0,
    Locked = 1,
  };

 public:
  void Lock() {
    while (state_.exchange(State::Locked) == State::Locked) {
      twist::ed::futex::Wait(state_, State::Locked);
    }
  }

  void Unlock() {
    auto waiters = twist::ed::futex::PrepareWake(state_);
    state_.store(State::Free);
    twist::ed::futex::WakeOne(waiters);
  }

 private:
  twist::ed::std::atomic_uint32_t state_{State::Free};
};
