#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/atomic_flag.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::atomic_flag;

}  // namespace twist::rt::cap::std_like

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/std_like/atomic_flag.hpp>

namespace twist::rt::cap::std_like {

using atomic_flag = rt::thr::fault::FaultyAtomicFlag;  // NOLINT

}  // namespace twist::rt::cap::std_like

#else

#include <atomic>

namespace twist::rt::cap::std_like {

using ::std::atomic_flag;

}  // namespace twist::rt::cap::std_like

#endif
