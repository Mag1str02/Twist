#pragma once

#include <cstdlib>
#include <thread>

namespace twist::rt::thr {

namespace cores::multi {

class [[nodiscard]] SpinWait {
 public:
  SpinWait() = default;

  // Non-copyable
  SpinWait(const SpinWait&) = delete;
  SpinWait& operator=(const SpinWait&) = delete;

  // Non-movable
  SpinWait(SpinWait&&) = delete;
  SpinWait& operator=(SpinWait&&) = delete;


  void Spin();

  void operator()() {
    Spin();
  }

  [[nodiscard]] bool ConsiderParking() const;

  [[nodiscard]] bool KeepSpinning() const {
    return !ConsiderParking();
  }

 private:
  size_t count_ = 0;
};

}  // namespace cores::multi

}  // namespace twist::rt::thr
