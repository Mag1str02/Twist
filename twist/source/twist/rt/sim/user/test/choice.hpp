#pragma once

#include <twist/rt/sim/user/syscall/random.hpp>

#include <wheels/core/assert.hpp>

namespace twist::rt::sim {

namespace user::test {

inline size_t Choice(size_t alts) {
  WHEELS_VERIFY(alts > 0, "Positive number of alternatives expected");
  return syscall::RandomChoice(alts);
}

inline size_t Choice(size_t lo, size_t hi) {
  WHEELS_VERIFY(lo < hi, "Non-empty range expected");
  return lo + syscall::RandomChoice(hi - lo);
}

}  // namespace user::test

}  // namespace twist::rt::sim
