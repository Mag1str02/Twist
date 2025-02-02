#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/shared_mutex.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::shared_mutex;

}  // namespace twist::rt::cap::std_like

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/std_like/shared_mutex.hpp>

namespace twist::rt::cap::std_like {

using shared_mutex = rt::thr::fault::FaultySharedMutex;  // NOLINT

}  // namespace twist::rt::cap::std_like

#else

#include <shared_mutex>

namespace twist::rt::cap::std_like {

using ::std::shared_mutex;

}  // namespace twist::rt::cap::std_like

#endif
