#pragma once

#if defined(__TWIST_SIM__)

#include <twist/ed/std/random.hpp>

#include <twist/rt/sim/user/assist/choice.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::sim::user::assist::Choice;

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/ed/std/random.hpp>

#include <twist/rt/thr/assist/choice.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::thr::assist::Choice;

}  // namespace assist

}  // namespace twist::rt::cap

#endif
