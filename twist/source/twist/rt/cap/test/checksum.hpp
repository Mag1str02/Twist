#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/checksum.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::user::test::CheckSum;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/checksum.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::test::CheckSum;

}  // namespace test

}  // namespace twist::rt::cap

#endif
