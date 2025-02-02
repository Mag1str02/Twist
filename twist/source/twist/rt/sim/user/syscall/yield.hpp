#pragma once

#include <twist/rt/sim/system/call/context.hpp>

namespace twist::rt::sim {

namespace user::syscall {

void Yield(const system::UserContext*);

}  // namespace user::syscall

}  // namespace twist::rt::sim
