#pragma once

#include "../simulator.hpp"
#include "replay.hpp"

#include <twist/rt/sim/scheduler/random/scheduler.hpp>

#include <optional>

namespace twist::sim::sched {

using RandomScheduler = twist::rt::sim::system::scheduler::random::Scheduler;

}  // namespace twist::sim::sched
