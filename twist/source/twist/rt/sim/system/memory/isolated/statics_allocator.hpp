#pragma once

#include "alignment.hpp"
#include "segment.hpp"
#include "panic.hpp"

#include <cstring>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

class StaticsAllocator {
 public:
  StaticsAllocator(Segment s)
      : segment_(s) {
    Reset();
  }

  void Reset() {
    next_ = Align(segment_.Begin());
  }

  void* Allocate(size_t size) {
    if (Overflow()) {
      return nullptr;
    }
    if (SpaceLeft() < size) {
      return nullptr;  // Overflow
    }

    void* addr = next_;
    std::memset(addr, 0, size);
    next_ = Align(next_ + size);
    return addr;
  }

 private:
  // Precondition: !Overflow()
  size_t SpaceLeft() const {
    WHEELS_ASSERT(!Overflow(), "Precondition failed");
    return segment_.End() - next_;
  }

  bool Overflow() const {
    return next_ >= segment_.End();
  }

 private:
  Segment segment_;
  char* next_;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
