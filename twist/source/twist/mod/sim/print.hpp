#pragma once

#include "simulator.hpp"
#include "sched/replay.hpp"
#include "log/pretty_printer.hpp"

namespace twist::sim {

// Backward compatibility
using StdoutPrinter = log::PrettyPrinter;

void Print(MainRoutine main, sched::Schedule schedule, SimulatorParams params = SimulatorParams{});

}  // namespace twist::sim
