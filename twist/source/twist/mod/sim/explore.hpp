#pragma once

#include "simulator.hpp"
#include "sched/replay.hpp"

namespace twist::sim {

struct Found {
  sched::Schedule schedule;
  Result result;
};

struct ExploreResult {
  std::optional<Found> found;
  size_t sims;
};

ExploreResult Explore(IScheduler& scheduler, SimulatorParams params, MainRoutine main);
ExploreResult Explore(IScheduler& scheduler, MainRoutine main);

}  // namespace twist::sim
