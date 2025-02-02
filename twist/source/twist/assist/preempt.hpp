#pragma once

/*
 * Preemption point for thread adversary / simulation scheduler
 *
 * void twist::assist::PreemptionPoint();
 *
 * {
 *   twist::assist::NoPreemptionGuard hint;
 *
 *   // ...
 * }
 *
 */

#include <twist/rt/cap/assist/preempt.hpp>

namespace twist::assist {

using rt::cap::assist::PreemptionPoint;
using rt::cap::assist::NoPreemptionGuard;

}  // namespace twist::assist
