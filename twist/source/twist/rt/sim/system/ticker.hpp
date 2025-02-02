#pragma once

#include <cstdint>

namespace twist::rt::sim {

namespace system {

using Ticks = uint64_t;

class Ticker {
 public:
  void Tick(Ticks d = 1) {
    count_ += d;
  }

  void Reset() {
    count_ = 0;
  }

  Ticks Count() const {
    return count_;
  }

 private:
  Ticks count_ = 0;
};

}  // namespace system

}  // namespace twist::rt::sim