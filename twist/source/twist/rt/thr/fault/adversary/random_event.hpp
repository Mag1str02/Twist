#pragma once

#include <twist/rt/thr/fault/random/range.hpp>

#include <atomic>

namespace twist::rt::thr {
namespace fault {

class RandomEvent {
 public:
  RandomEvent(size_t freq) : freq_(freq) {
    Init();
  }

  // Wait-free
  bool Test() {
    if (left_.fetch_sub(1, std::memory_order::relaxed) == 1) {
      // Last tick
      Reset();
      return true;
    }
    return false;
  }

  void Reset() {
    left_.store(RandomUInteger(1, freq_), std::memory_order::relaxed);
  }

  void Init() {
    left_ = 1;  // TODO
  }

 private:
  size_t freq_;
  ::std::atomic<int> left_{0};
};

}  // namespace fault
}  // namespace twist::rt::thr
