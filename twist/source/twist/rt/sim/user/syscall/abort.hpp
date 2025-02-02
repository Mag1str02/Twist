#pragma once

#include <twist/rt/sim/system/status.hpp>

namespace twist::rt::sim {

namespace user::syscall {

[[noreturn]] void Abort(system::Status);

}  // namespace user::syscall

}  // namespace twist::rt::sim
