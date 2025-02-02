#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/wait_group.hpp>

namespace twist::rt::cap {

namespace test {

using sim::user::test::WaitGroup;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/wait_group.hpp>

namespace twist::rt::cap {

namespace test {

using thr::test::WaitGroup;

}  // namespace test

}  // namespace twist::rt::cap

#endif
