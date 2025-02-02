#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/choice.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::user::test::Choice;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/random/choice.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::random::Choice;

}  // namespace test

}  // namespace twist::rt::cap

#endif
