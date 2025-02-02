#pragma once

#include <cstdint>
#include <cstdlib>
#include <optional>

namespace twist::rt::sim {

namespace system::scheduler::random {

struct Rational {
  uint64_t num;
  uint64_t denom;
};

struct Params {
  // Seed for PRNG
  uint64_t seed;
  // Number of simulations to run
  std::optional<size_t> max_runs;
  // Probability of spurious wakeup
  Rational spurious_wakeups = {1, 7};
  // Probability of spurious compare_exchange_weak / try_lock failure
  Rational spurious_failures = {1, 7};
  // Fiber time slice is RandomNumber(1, {slice}) sync actions
  size_t time_slice = 15;

  // Lock-free support
  bool lock_free = true;
  size_t lf_progress_streak_to_resume = 4;
  // Hard limit for number of suspended threads
  size_t lf_suspended_threads_limit = 1;
};

}  // namespace system::scheduler::random

}  // namespace twist::rt::sim
