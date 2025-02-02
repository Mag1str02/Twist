#pragma once

#include <twist/rt/sim/system/futex/loc.hpp>
#include <twist/rt/sim/system/futex/wake_count.hpp>
#include <twist/rt/sim/system/futex/waiter.hpp>
#include <twist/rt/sim/system/futex/waker.hpp>
#include <twist/rt/sim/system/call/status.hpp>
#include <twist/rt/sim/system/time.hpp>

namespace twist::rt::sim {

namespace user::syscall {

system::call::Status FutexWait(system::FutexLoc loc, uint64_t val, system::WaiterContext*);
system::call::Status FutexWaitTimed(system::FutexLoc loc, uint64_t val, system::Time::Instant, system::WaiterContext*);
void FutexWake(system::FutexLoc, size_t, system::WakerContext*);

}  // namespace user::syscall

}  // namespace twist::rt::sim
