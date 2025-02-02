#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/unique_lock.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::unique_lock;

}  // namespace twist::rt::cap::std_like

#else

#include <mutex>

namespace twist::rt::cap::std_like {

using ::std::unique_lock;

}  // namespace twist::rt::cap::std_like

#endif
