#pragma once

#include <twist/rt/sim/system/thread/id.hpp>
#include <twist/rt/sim/system/thread/user.hpp>
#include <twist/rt/sim/system/call/context.hpp>

namespace twist::rt::sim {

namespace user::syscall {

system::ThreadId Spawn(system::IThreadUserState*, const system::UserContext*);
void Detach(system::ThreadId);

}  // namespace user::syscall

}  // namespace twist::rt::sim
