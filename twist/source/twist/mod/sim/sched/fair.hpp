#pragma once

#include "../simulator.hpp"

#include <twist/rt/sim/scheduler/random/scheduler.hpp>

namespace twist::sim::sched {

using FairScheduler = twist::rt::sim::system::scheduler::random::Scheduler;

}  // namespace twist::sim::sched
