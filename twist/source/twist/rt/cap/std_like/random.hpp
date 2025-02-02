#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/random.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::random_device;

}  // namespace twist::rt::cap::std_like

#else

#include <random>

namespace twist::rt::cap::std_like {

using ::std::random_device;

}  // namespace twist::rt::cap::std_like

#endif
