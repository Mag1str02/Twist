#pragma once

#include <twist/rt/thr/fault/adversary/random_event.hpp>

#include <thread>
#include <atomic>

namespace twist::rt::thr {
namespace fault {

/////////////////////////////////////////////////////////////////////

class Yielder {
 public:
  Yielder(size_t freq) : yield_(freq) {
  }

  void Reset() {
    yield_.Init();
    yield_count_ = 0;
  }

  void MaybeYield() {
    if (yield_.Test()) {
      ::std::this_thread::yield();
      yield_count_.fetch_add(1, std::memory_order::relaxed);
    }
  }

  size_t YieldCount() const {
    return yield_count_.load(std::memory_order::relaxed);
  }

 private:
  RandomEvent yield_;

  // statistics
  ::std::atomic<size_t> yield_count_{0};
};

}  // namespace fault
}  // namespace twist::rt::thr
