#pragma once

#include "segment.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

class SharedSegment {
 public:
  SharedSegment(Segment segment)
      : segment_(segment) {
    Reset();
  }

  void Reset() {
    up_ = segment_.Begin();
    down_ = segment_.End();
  }

  char* Begin() const {
    return segment_.Begin();
  }

  char* UpPtr() const {
    return up_;
  }

  char* DownPtr() const {
    return down_;
  }

  // ~ brk
  bool TrySetUp(char* ptr) {
    WHEELS_ASSERT(ptr >= up_, "Pointer should be at least UpPtr()");
    return TryUp(ptr - up_) != nullptr;
  }

  // ~ sbrk
  // Returns _old_ up address on success
  char* TryUp(size_t size) {
    if (SpaceLeft() < size) {
      return nullptr;
    }
    char* old_up = up_;
    up_ += size;
    return old_up;
  }

  // Returns _new_ down address on success
  char* TryDown(size_t size) {
    if (SpaceLeft() < size) {
      return nullptr;
    }
    down_ -= size;
    return down_;
  }

  size_t SpaceLeft() const {
    return down_ - up_;
  }

 private:
  wheels::MutableMemView segment_;
  char* up_;
  char* down_;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
