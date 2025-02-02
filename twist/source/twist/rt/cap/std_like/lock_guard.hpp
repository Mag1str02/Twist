#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/lock_guard.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::lock_guard;

}  // namespace twist::rt::cap::std_like

#else

#include <mutex>

namespace twist::rt::cap::std_like {

using ::std::lock_guard;

}  // namespace twist::rt::cap::std_like

#endif
