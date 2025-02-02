#pragma once

#include "replay.hpp"

#include "../simulator.hpp"

#include <twist/rt/sim/scheduler/dfs/scheduler.hpp>
#include <twist/rt/sim/scheduler/replay/schedule.hpp>

#include <optional>

namespace twist::sim::sched {

using DfsScheduler = twist::rt::sim::system::scheduler::dfs::Scheduler;

}  // namespace twist::sim::sched
