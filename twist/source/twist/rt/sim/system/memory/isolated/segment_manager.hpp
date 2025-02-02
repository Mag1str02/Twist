#pragma once

#include "segment.hpp"
#include "shared_segment.hpp"

#include <wheels/core/assert.hpp>

#include <optional>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

class SegmentManager {
  static const size_t kStaticSegmentSize = 64 * 1024;
  static const size_t kTooSmall = 1024 * 1024;

 public:
  SegmentManager(wheels::MutableMemView memory) {
    WHEELS_VERIFY(memory.Size() > kTooSmall, "Memory arena is too small for simulation");

    static_ = {memory.Begin(), kStaticSegmentSize};
    memory += kStaticSegmentSize;
    dynamic_.emplace(memory);
  }

  void Reset() {
  }

  Segment Static() {
    return static_;
  }

  SharedSegment* Dynamic() {
    return &(*dynamic_);
  }

 private:
  Segment static_;
  std::optional<SharedSegment> dynamic_;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
