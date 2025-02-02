#pragma once

#include "thread/id.hpp"
#include "limits.hpp"

#include <wheels/core/assert.hpp>

namespace twist::rt::sim {

namespace system {

class IdAllocator {
 public:
  ThreadId AllocateId() {
    WHEELS_VERIFY(next_id_ < kMaxThreads, "Too many threads");
    return ++next_id_;
  }

  static constexpr ThreadId kImpossibleId = 0;

  void Free(ThreadId) {
  }

  void Reset() {
    next_id_ = 0;
  }

 private:
  uint8_t next_id_ = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
