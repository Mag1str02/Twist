#pragma once

#include "type.hpp"
#include "../thread/id.hpp"

#include <wheels/core/source_location.hpp>

#include <optional>

namespace twist::rt::sim {

namespace system {

struct WaiterContext {
  FutexType type;
  const char* operation;
  wheels::SourceLocation source_loc;
  std::optional<ThreadId> waiting_for{};
};

}  // namespace system

}  // namespace twist::rt::sim
