#pragma once

#include <twist/rt/sim/system/fwd.hpp>
#include <twist/rt/sim/system/limits.hpp>

#include <cstdint>

namespace twist::rt::sim {

namespace system::scheduler::random {

// Deque-based sim queue with fast O(1) PickRandom

class ThreadSet {
 public:
  void Reset() {
    size_ = 0;
  }

  void Push(Thread* thread) {
    WHEELS_VERIFY(!IsFull(), "Twist thread limit reached: " << kMaxThreads);
    buf_[size_++] = thread;
  }

  size_t Size() const {
    return size_;
  }

  bool IsEmpty() const {
    return size_ == 0;
  }

  bool IsFull() const {
    return size_ == kMaxThreads;
  }

  Thread* PopAll() {
    if (size_ == 0) {
      return nullptr;
    }
    Thread* pop = buf_[size_ - 1];
    --size_;
    return pop;
  }

  // For fault injection, O(1)
  Thread* PopRandom(uint64_t r) {
    if (size_ == 0) {
      return nullptr;
    }

    size_t index = r % size_;

    Thread* thread = buf_[index];
    buf_[index] = buf_[size_ - 1];
    --size_;

    return thread;
  }

  // O(size),
  // For timeouts, so no reason to optimize
  bool Remove(Thread* thread) {
    for (size_t i = 0; i < size_; ++i) {
      if (buf_[i] == thread) {
        buf_[i] = buf_[size_ - 1];
        --size_;
        return true;
      }
    }

    return false;  // Not found
  }

 private:
  std::array<Thread*, kMaxThreads> buf_;
  size_t size_ = 0;
};

}  // namespace system::scheduler::random

}  // twist::rt::sim
