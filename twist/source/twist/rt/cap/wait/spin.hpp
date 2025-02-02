#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/wait/spin.hpp>

namespace twist::rt::cap {

using rt::sim::user::SpinWait;
using rt::sim::user::CpuRelax;

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/wait/spin/wait.hpp>

namespace twist::rt::cap {

using rt::thr::SpinWait;
using rt::thr::CpuRelax;

}  // namespace twist::rt::cap

#endif
