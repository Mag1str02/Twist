#pragma once

/*
 * void assist::Prune(std::string_view why)
 *
 * Prune current simulation if possible
 * (twist::build::IsolatedSim()),
 *
 * NB: Not compatible with preemption-bounded DPOR!
 *
 * Usage:
 *
 * while (!flag.load()) {
 *   twist::assist::Prune("Spin loop");
 * }
 *
 */

#include <twist/rt/cap/assist/prune.hpp>

namespace twist::assist {

using rt::cap::assist::Prune;

}  // namespace twist::assist
