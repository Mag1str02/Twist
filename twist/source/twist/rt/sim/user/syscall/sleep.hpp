#pragma once

#include <twist/rt/sim/system/time.hpp>

namespace twist::rt::sim {

namespace user::syscall {

void SleepFor(system::Time::Duration delay);
void SleepUntil(system::Time::Instant deadline);

}  // namespace user::syscall

}  // namespace twist::rt::sim
