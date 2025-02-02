#pragma once

#include <twist/rt/sim/user/syscall/random.hpp>

namespace twist::rt::sim {

namespace user::test {

inline bool Either() {
  return syscall::RandomChoice(2) == 0;
}

inline size_t Either(size_t alts) {
  return syscall::RandomChoice(alts);
}

}  // namespace user::test

}  // namespace twist::rt::sim
