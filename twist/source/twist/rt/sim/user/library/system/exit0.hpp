#pragma once

#include <twist/rt/sim/user/syscall/abort.hpp>

namespace twist::rt::sim {

namespace user::library::system {

[[noreturn]] inline void Exit0() {
  syscall::Abort(sim::system::Status::Ok);
}

}  // namespace user::library::system

}  // namespace twist::rt::sim
