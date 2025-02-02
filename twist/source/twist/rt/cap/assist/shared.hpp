#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/shared.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::sim::user::assist::Shared;

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/assist/shared.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::thr::assist::Shared;

}  // namespace assist

}  // namespace twist::rt::cap

#endif
