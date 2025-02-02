#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/inject_fault.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::user::test::InjectFault;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/inject_fault.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::test::InjectFault;

}  // namespace test

}  // namespace twist::rt::cap

#endif
