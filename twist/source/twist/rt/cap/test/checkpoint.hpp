#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/checkpoint.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::user::test::Checkpoint;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/checkpoint.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::test::Checkpoint;

}  // namespace test

}  // namespace twist::rt::cap

#endif
