#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/preempt.hpp>

namespace twist::rt::cap::assist {

using rt::sim::user::assist::PreemptionPoint;
using rt::sim::user::assist::NoPreemptionGuard;

}  // namespace twist::rt::cap::assist

#else

#include <twist/rt/thr/assist/preempt.hpp>

namespace twist::rt::cap::assist {

using rt::thr::assist::PreemptionPoint;
using rt::thr::assist::NoPreemptionGuard;

}  // namespace twist::rt::cap::assist

#endif
