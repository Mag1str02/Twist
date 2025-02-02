#pragma once

#include <twist/rt/sim/user/syscall/abort.hpp>

namespace twist::rt::sim {

namespace user::library::c {

[[noreturn]] inline void abort() {
  syscall::Abort(sim::system::Status::UserAbort);
}

}  // namespace user::library::c

}  // namespace twist::rt::sim
