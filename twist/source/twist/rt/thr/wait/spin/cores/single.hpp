#pragma once

#include <thread>

namespace twist::rt::thr {

namespace cores::single {

class [[nodiscard]] SpinWait {
 public:
  SpinWait() = default;

  // Non-copyable
  SpinWait(const SpinWait&) = delete;
  SpinWait& operator=(const SpinWait&) = delete;

  // Non-movable
  SpinWait(SpinWait&&) = delete;
  SpinWait& operator=(SpinWait&&) = delete;


  void Spin() {
    ++yield_count_;
    std::this_thread::yield();
  }

  [[nodiscard]] bool ConsiderParking() const {
    static const size_t kYieldLimit = 4;  // Arbitrary

    return yield_count_ > kYieldLimit;
  }

  [[nodiscard]] bool KeepSpinning() const {
    return !ConsiderParking();
  }

  void operator()() {
    Spin();
  }

 private:
  size_t yield_count_ = 0;
};

}  // namespace cores::single

}  // namespace twist::rt::thr
