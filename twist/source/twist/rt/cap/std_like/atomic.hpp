#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/atomic.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::atomic;

}  // namespace twist::rt::cap::std_like

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/std_like/atomic.hpp>

namespace twist::rt::cap::std_like {

template <typename T>
using atomic = rt::thr::fault::FaultyAtomic<T>;  // NOLINT

}  // namespace twist::rt::cap::std_like

#else

#include <atomic>

namespace twist::rt::cap::std_like {

using ::std::atomic;

}  // namespace twist::rt::cap::std_like

#endif
