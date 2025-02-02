#pragma once

#include "wake_key.hpp"

namespace futex_like {

namespace atomic {

template <typename AtomicRef>
WakeKey PrepareWake(AtomicRef atom_ref) {
  return {atom_ref.FutexLoc()};
}

}  // namespace atomic

}  // namespace futex_like
