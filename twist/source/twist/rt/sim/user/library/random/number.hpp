#pragma once

#include <twist/rt/sim/user/syscall/random.hpp>

namespace twist::rt::sim {

namespace user::library::random {

inline uint64_t Number() {
  return syscall::RandomNumber();
}

}  // namespace user::library::random

}  // namespace twist::rt::sim
