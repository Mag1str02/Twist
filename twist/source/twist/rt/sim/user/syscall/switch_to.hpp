#pragma once

#include <twist/rt/sim/system/thread/id.hpp>

#include <wheels/core/source_location.hpp>
#include <wheels/core/compiler.hpp>

namespace twist::rt::sim {

namespace user::syscall {

void SwitchTo(system::ThreadId target);

}  // namespace user::syscall

}  // namespace twist::rt::sim
