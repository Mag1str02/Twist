#pragma once

#include <twist/rt/sim/user/syscall/random.hpp>
#include <twist/rt/sim/user/library/std_like/random.hpp>

#include <wheels/core/assert.hpp>

#include <cstdlib>

namespace twist::rt::sim {

namespace user::assist {

class Choice {
 public:
  explicit Choice(user::library::std_like::random_device& /*rd*/) {
    // No-op
  }

  // Non-copyable
  Choice(const Choice&) = delete;
  Choice& operator=(const Choice&) = delete;

  // Non-movable
  Choice(Choice&&) = delete;
  Choice& operator=(Choice&&) = delete;

  // [0, alts)
  size_t operator()(size_t alts) {
    WHEELS_ASSERT(alts > 0, "Positive number of alternatives expected");
    return Impl(alts);
  }

  // [lo, hi)
  size_t operator()(size_t lo, size_t hi) {
    WHEELS_ASSERT(lo < hi, "Invalid index range");
    return lo + Impl(hi - lo);
  }

 private:
  size_t Impl(size_t alts) {
    return syscall::RandomChoice(alts);
  }
};

}  // namespace user::assist

}  // namespace twist::rt::sim
