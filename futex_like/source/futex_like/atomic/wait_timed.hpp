#pragma once

#include <futex_like/system/wait.hpp>

#include <cassert>
#include <chrono>

namespace futex_like {

namespace atomic {

inline std::chrono::milliseconds ToMilliSeconds(std::chrono::nanoseconds ns) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(ns);
}

// WaitTimed

template <typename Uint>
using WaitTimedStatus = std::pair<bool, Uint>;

template <typename AtomicRef>
WaitTimedStatus<typename AtomicRef::AtomicValueType> WaitTimed(
    AtomicRef atom_ref, uint32_t old, std::chrono::milliseconds timeout,
    std::memory_order mo) {
  using Clock = std::chrono::steady_clock;

  if (timeout.count() == 0) {
    return {false, 0};  // Zero timeout
  }

  const auto start = Clock::now();
  const auto deadline = start + timeout;

  auto now = start;

  while (true) {
    {
      // WaitTimed

      assert(deadline >= now);
      const std::chrono::milliseconds left = ToMilliSeconds(deadline - now);
      system::WaitTimed(atom_ref.FutexLoc(), old, /*millis=*/left.count() + 1);
    }

    {
      // Wake-up?

      typename AtomicRef::AtomicValueType atom_curr_value =
          atom_ref.AtomicLoad(mo);

      if (AtomicRef::GetFutexValue(atom_curr_value) != old) {
        return {true, atom_curr_value};  // Wake
      }
    }

    {
      // Timeout?

      // Update now
      now = Clock::now();

      if (now >= deadline) {
        return {false, 0};  // Timeout
      }
    }
  }
}

}  // namespace atomic

}  // namespace futex_like
