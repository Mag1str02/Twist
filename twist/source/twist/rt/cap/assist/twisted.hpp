#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/twisted.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::sim::user::assist::Twisted;

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/assist/twisted.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::thr::assist::Twisted;

}  // namespace assist

}  // namespace twist::rt::cap

#endif
