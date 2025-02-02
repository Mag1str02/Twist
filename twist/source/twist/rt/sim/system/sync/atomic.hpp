#pragma once

#include "action.hpp"
#include "clock.hpp"
#include "mo.hpp"

#include <cstdint>
#include <optional>

namespace twist::rt::sim {

namespace system::sync {

struct AtomicStore {
  VectorClock clock;
  uint64_t value;
};

struct AtomicDtor {
  //
};

struct AtomicVar {
  wheels::SourceLocation source_loc;
  uint8_t size;
  AtomicStore last_store;
  std::optional<AtomicDtor> dtor;
};

}  // namespace system::sync

}  // namespace twist::rt::sim
