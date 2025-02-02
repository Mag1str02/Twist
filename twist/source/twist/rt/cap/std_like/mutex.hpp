#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/mutex.hpp>
#include <twist/rt/sim/user/library/std_like/timed_mutex.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::mutex;
using rt::sim::user::library::std_like::timed_mutex;

}  // namespace twist::rt::cap::std_like

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/std_like/mutex.hpp>
#include <twist/rt/thr/fault/std_like/timed_mutex.hpp>

namespace twist::rt::cap::std_like {

using mutex = rt::thr::fault::FaultyMutex;  // NOLINT
using timed_mutex = rt::thr::fault::FaultyTimedMutex;  // NOLINT

}  // namespace twist::rt::cap::std_like

#else

#include <mutex>

namespace twist::rt::cap::std_like {

using ::std::mutex;
using ::std::timed_mutex;

}  // namespace twist::rt::cap::std_like

#endif
