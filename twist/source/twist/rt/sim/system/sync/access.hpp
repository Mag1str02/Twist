#pragma once

#include "clock.hpp"
#include "../thread/id.hpp"

#include <wheels/core/source_location.hpp>

#include <cstdint>

namespace twist::rt::sim {

namespace system::sync {

// Access type

enum class AccessType : uint8_t {
  Init = 0,
  Read = 1,
  Write = 2,
  Destroy = 3,
};

inline bool IsRead(AccessType a) {
  return a == AccessType::Read;
}

inline bool IsWrite(AccessType a) {
  return !IsRead(a);
}

// Current access

struct Access {
  void* loc;  // Memory location
  AccessType type;
  wheels::SourceLocation source_loc;
};

// Old access

struct OldAccess {
  void* loc;
  AccessType type;
  ThreadId thread = 0;
  Epoch epoch = 0;
  wheels::SourceLocation source_loc;
};

}  // namespace system::sync

}  // namespace twist::rt::sim
