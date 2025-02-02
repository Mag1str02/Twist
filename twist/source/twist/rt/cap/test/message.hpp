#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/message.hpp>

namespace twist::rt::cap {

namespace test {

using rt::sim::test::Message;

}  // namespace test

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/test/message.hpp>

namespace twist::rt::cap {

namespace test {

using rt::thr::test::Message;

}  // namespace test

}  // namespace twist::rt::cap

#endif
