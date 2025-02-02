#pragma once

#include "clock.hpp"
#include "../thread/id.hpp"

#include <wheels/core/source_location.hpp>

#include <optional>

namespace twist::rt::sim {

namespace system::sync {

struct Alloc {
  bool alive;
  void* addr;
  size_t size;
  ThreadId thread_id;
  std::optional<wheels::SourceLocation> source_loc;
  VectorClock free;
};

}  // namespace system::sync

}  // namespace twist::rt::sim
