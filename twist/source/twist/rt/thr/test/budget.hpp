#pragma once

#include <atomic>

namespace twist::rt::thr {

namespace test {

class Budget {
 public:
  explicit Budget(size_t count)
    : left_(count) {
  }

  bool Withdraw(size_t count) {
    size_t curr = left_.load(std::memory_order::relaxed);

    while (true) {
      if (curr < count) {
        return false;
      }
      if (left_.compare_exchange_strong(curr, curr - count)) {
        return true;
      }
      // Continue
    }
  }

 private:
  std::atomic<size_t> left_;
};

}  // namespace user::test

}  // namespace twist::rt::thr
