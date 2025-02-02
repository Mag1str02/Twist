#pragma once

#include <wheels/core/source_location.hpp>

namespace twist::rt::sim {

namespace system {

struct WakerContext {
  const char* operation;
  wheels::SourceLocation source_loc;
};

}  // namespace system

}  // namespace twist::rt::sim
