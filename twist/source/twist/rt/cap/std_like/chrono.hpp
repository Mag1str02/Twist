#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/chrono.hpp>

namespace twist::rt::cap::std_like::chrono {

using system_clock = rt::sim::user::library::std_like::chrono::Clock;  // NOLINT
using steady_clock = rt::sim::user::library::std_like::chrono::Clock;  // NOLINT
using high_resolution_clock = rt::sim::user::library::std_like::chrono::Clock;  // NOLINT

}  // namespace twist::rt::cap::std_like::chrono

#else

#include <chrono>

namespace twist::rt::cap::std_like::chrono {

using ::std::chrono::system_clock;
using ::std::chrono::steady_clock;
using ::std::chrono::high_resolution_clock;

}  // namespace twist::rt::cap::std_like::chrono

#endif
