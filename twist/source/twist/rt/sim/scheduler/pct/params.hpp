#pragma once

#include <cstdint>
#include <cstdlib>

#include <optional>

namespace twist::rt::sim {

namespace system::scheduler::pct {

struct Estimates {
  size_t threads;
  size_t steps;
};

struct Params {
  std::optional<Estimates> hints;
  // Number of priority change points
  size_t depth;
  // Seed for PRNG
  uint64_t seed = 42;
  std::optional<size_t> max_runs;
};

}  // namespace system::scheduler::pct

}  // namespace twist::rt::sim
