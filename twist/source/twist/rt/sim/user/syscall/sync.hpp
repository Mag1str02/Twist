#pragma once

#include <twist/rt/sim/system/sync/action.hpp>

namespace twist::rt::sim {

namespace user::syscall {

uint64_t Sync(system::sync::Action* action);

}  // namespace user::syscall

}  // namespace twist::rt::sim
