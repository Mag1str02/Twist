#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/c/abort.hpp>

namespace twist::rt::cap {

namespace c {

using sim::user::library::c::abort;

}  // namespace c

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/c/abort.hpp>

namespace twist::rt::cap {

namespace c {

using thr::abort;

}  // namespace c

}  // namespace twist::rt::cap

#endif
