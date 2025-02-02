#pragma once

#include <cstdlib>
#include <optional>
#include <string>

namespace twist::rt::sim {

namespace system::scheduler::dfs {

struct Params {
  std::optional<size_t> max_preemptions;
  std::optional<size_t> max_steps;
  std::optional<size_t> force_preempt_after_steps;
  bool wake_order = true;
  bool spurious_wakeups = false;
  bool spurious_failures = false;
  std::optional<std::string> checkpoint_file;  // TODO
};

}  // namespace system::scheduler::dfs

}  // namespace twist::rt::sim
