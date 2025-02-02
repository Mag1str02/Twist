#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/prune.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::sim::user::assist::Prune;

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/assist/prune.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::thr::assist::Prune;

}  // namespace assist

}  // namespace twist::rt::cap

#endif
