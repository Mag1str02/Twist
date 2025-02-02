#pragma once

#include <twist/rt/thr/wait/futex/wait.hpp>

#include <twist/rt/thr/fault/std_like/thread.hpp>

#include <twist/rt/thr/logging/logging.hpp>

#include <wheels/core/compiler.hpp>

#include <atomic>
#include <type_traits>
#include <vector>

namespace twist::rt::thr {

namespace test {

class WgLatch {
 public:
  void Wait() {
    futex::Wait(released_, 0);
  }

  void Release() {
    auto wake_key = futex::PrepareWake(released_);
    released_.store(1);
    futex::WakeAll(wake_key);
  }

 private:
  std::atomic<uint32_t> released_{0};
};

class WaitGroup {
 public:
  template <typename F>
  WaitGroup& Add(F fn) {
    size_t index = threads_.size();
    threads_.emplace_back([this, index, fn = std::move(fn)]() mutable {
      start_latch_.Wait();

      if constexpr (std::is_invocable_v<F, size_t>) {
        fn(index);
      } else {
        WHEELS_UNUSED(index);
        fn();
      }
    });
    return *this;
  }

  template <typename F>
  WaitGroup& Add(size_t count, F fn) {
    for (size_t i = 0; i < count; ++i) {
      Add(fn);
    }
    return *this;
  }

  void Join() {
    {
      // Start participants
      start_latch_.Release();
    }

    {
      // Join participants
      for (auto& t : threads_) {
        t.join();
      }
      threads_.clear();
    }

    rt::thr::log::FlushPendingLogMessages();
  }

 private:
  WgLatch start_latch_;
  std::vector<fault::FaultyThread> threads_;
};

}  // namespace test

}  // namespace twist::rt::thr
