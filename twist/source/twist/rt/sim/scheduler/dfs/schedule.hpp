#pragma once

#include <cstdint>
#include <vector>

namespace twist::rt::sim {

namespace system::scheduler::dfs {

enum BranchType : uint32_t {
  PickNext,
  WakeOne,
  Preempt,
  RandomChoice,
  SpuriousWakeup,
  SpuriousTryFailure,
};

struct Branch {
  BranchType type;
  size_t alts;
  size_t index;
};

using Schedule = std::vector<Branch>;

}  // namespace system::scheduler::dfs

}  // namespace twist::rt::sim
