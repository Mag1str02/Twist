#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/test/lock_free.hpp>

namespace twist::rt::cap::test {

using rt::sim::user::test::SetThreadAttrLockFree;
using rt::sim::user::test::LockFreeScope;

using rt::sim::user::test::Progress;

}  // namespace twist::rt::cap::test

#else

#include <twist/rt/thr/test/lock_free.hpp>

namespace twist::rt::cap::test {

using rt::thr::test::SetThreadAttrLockFree;
using rt::thr::test::LockFreeScope;

using rt::thr::test::Progress;

}  // namespace twist::rt::cap::test

#endif
