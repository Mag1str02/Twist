#pragma once

#include "action_type.hpp"
#include "value.hpp"

#include "../thread/id.hpp"

#include <wheels/core/source_location.hpp>

// std::memory_order
#include <atomic>
#include <cstdint>

namespace twist::rt::sim {

namespace system::sync {

struct Action {
  void* loc;  // Memory location
  uint8_t size;  // 4, 8
  ActionType type;  // AtomicLoad / AtomicStore / etc
  uint64_t value;
  std::memory_order mo;
  ValueType value_type;
  const char* operation;  // E.g. mutex::lock or atomic::store
  wheels::SourceLocation source_loc;
};

}  // namespace system::sync

}  // namespace twist::rt::sim
