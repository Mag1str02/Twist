#pragma once

#include <atomic>

#include <futex_like/wait_wake.hpp>

class Mutex {
  enum State : uint32_t {
    Unlocked = 0,
    Locked = 1,
  };

 public:
  void Lock() {
    while (!TryLock()) {
      futex_like::Wait(state_, State::Locked);
    }
  }

  bool TryLock() {
    return state_.exchange(State::Locked, std::memory_order::acquire) == State::Unlocked;
  }

  void Unlock() {
    auto waiters = futex_like::PrepareWake(state_);
    state_.store(State::Unlocked, std::memory_order::release);
    futex_like::WakeOne(waiters);
  }

  // Lockable

  void lock() {  // NOLINT
    Lock();
  }

  bool try_lock() {  // NOLINT
    return TryLock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  std::atomic_uint32_t state_{State::Unlocked};
};
