#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/fmt/print.hpp>

namespace twist::rt::cap {

namespace fmt {

using sim::user::library::fmt::print;
using sim::user::library::fmt::println;

}  // namespace fmt

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/fmt/print.hpp>

namespace twist::rt::cap {

namespace fmt {

using thr::fmt::print;
using thr::fmt::println;

}  // namespace fmt

}  // namespace twist::rt::cap

#endif
