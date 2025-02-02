#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/plate.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::user::test::Plate;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/plate.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::test::Plate;

}  // namespace test

}  // namespace twist::rt::cap

#endif
