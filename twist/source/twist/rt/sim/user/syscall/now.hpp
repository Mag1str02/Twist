#pragma once

#include <twist/rt/sim/system/time.hpp>

namespace twist::rt::sim {

namespace user::syscall {

system::Time::Instant Now();

}  // namespace user::syscall

}  // namespace twist::rt::sim
