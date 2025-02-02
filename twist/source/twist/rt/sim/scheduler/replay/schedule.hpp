#pragma once

#include <twist/rt/sim/system/thread/id.hpp>

#include <cstdint>
#include <cstdlib>
#include <optional>
#include <variant>
#include <vector>

namespace twist::rt::sim {

namespace system::scheduler::replay {

// Decisions

namespace decision {

struct PickNext {
  ThreadId id;
};

struct WakeOne {
  std::optional<ThreadId> id;
};

struct WakeAll {
  std::optional<ThreadId> id;
};

struct RandomNumber {
  uint64_t value;
};

struct RandomChoice {
  size_t index;
};

struct SpuriousWakeup {
  bool wake;
};

struct SpuriousTryFailure {
  bool fail;
};

}  // namespace decision

using Decision = std::variant<
    decision::PickNext,
    decision::WakeOne,
    decision::WakeAll,
    decision::RandomNumber,
    decision::RandomChoice,
    decision::SpuriousWakeup,
    decision::SpuriousTryFailure>;

// Schedule

using Schedule = std::vector<Decision>;

}  // namespace system::scheduler::replay

}  // namespace twist::rt::sim
