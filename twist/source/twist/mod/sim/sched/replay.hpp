#pragma once

#include "../simulator.hpp"

#include <twist/rt/sim/scheduler/replay/scheduler.hpp>
#include <twist/rt/sim/scheduler/replay/recorder.hpp>

namespace twist::sim::sched {

using Schedule = twist::rt::sim::system::scheduler::replay::Schedule;
using Recorder = twist::rt::sim::system::scheduler::replay::Recorder;
using ReplayScheduler = twist::rt::sim::system::scheduler::replay::Scheduler;

}  // namespace twist::sim::sched
