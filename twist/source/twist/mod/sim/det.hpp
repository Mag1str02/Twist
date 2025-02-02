#pragma once

#include "simulator.hpp"

namespace twist::sim {

bool DetCheck(SimulatorParams params, MainRoutine);

inline bool DetCheck(MainRoutine main) {
  return DetCheck({}, main);
}

}  // namespace twist::sim
