#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/wait/futex.hpp>

namespace twist::rt::cap {

namespace futex {

using rt::sim::user::library::futex::AtomicUint64LowHalfRef;
using rt::sim::user::library::futex::AtomicUint64HighHalfRef;

using rt::sim::user::library::futex::RefLowHalf;
using rt::sim::user::library::futex::RefHighHalf;

using rt::sim::user::library::futex::Wait;
using rt::sim::user::library::futex::WaitTimed;

using rt::sim::user::library::futex::WakeKey;

using rt::sim::user::library::futex::PrepareWake;

using rt::sim::user::library::futex::WakeOne;
using rt::sim::user::library::futex::WakeAll;

}  // namespace futex

}  // namespace twist::rt::cap

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/wait/futex.hpp>

namespace twist::rt::cap {

namespace futex {

using rt::thr::fault::futex::AtomicUint64LowHalfRef;
using rt::thr::fault::futex::AtomicUint64HighHalfRef;

using rt::thr::fault::futex::RefLowHalf;
using rt::thr::fault::futex::RefHighHalf;

using rt::thr::fault::futex::Wait;
using rt::thr::fault::futex::WaitTimed;

using rt::thr::fault::futex::WakeKey;

using rt::thr::fault::futex::PrepareWake;

using rt::thr::fault::futex::WakeOne;
using rt::thr::fault::futex::WakeAll;

}  // namespace futex

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/wait/futex/wait.hpp>

namespace twist::rt::cap {

namespace futex {

using rt::thr::futex::AtomicUint64LowHalfRef;
using rt::thr::futex::AtomicUint64HighHalfRef;

using rt::thr::futex::RefLowHalf;
using rt::thr::futex::RefHighHalf;

using rt::thr::futex::Wait;
using rt::thr::futex::WaitTimed;

using rt::thr::futex::WakeKey;

using rt::thr::futex::PrepareWake;

using rt::thr::futex::WakeOne;
using rt::thr::futex::WakeAll;

}  // namespace futex

}  // namespace twist::rt::cap

#endif
