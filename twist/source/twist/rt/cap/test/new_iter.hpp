#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/new_iter.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::user::test::NewIterHint;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/new_iter.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::test::NewIterHint;

}  // namespace test

}  // namespace twist::rt::cap

#endif
