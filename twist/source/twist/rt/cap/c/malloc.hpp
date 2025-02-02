#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/c/malloc.hpp>

namespace twist::rt::cap {

namespace c {

using sim::user::library::c::malloc;
using sim::user::library::c::free;

}  // namespace c

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/c/malloc.hpp>

namespace twist::rt::cap {

namespace c {

using thr::c::malloc;
using thr::c::free;

}  // namespace c

}  // namespace twist::rt::cap

#endif
