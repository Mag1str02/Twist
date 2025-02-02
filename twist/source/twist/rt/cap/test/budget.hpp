#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/budget.hpp>

namespace twist::rt::cap {

namespace test {

using sim::user::test::Budget;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/budget.hpp>

namespace twist::rt::cap {

namespace test {

using thr::test::Budget;

}  // namespace test

}  // namespace twist::rt::cap

#endif
