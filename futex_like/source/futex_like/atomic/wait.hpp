#pragma once

#include <futex_like/system/wait.hpp>

#include <cassert>

namespace futex_like {

namespace atomic {

template <typename AtomicRef>
typename AtomicRef::AtomicValueType Wait(AtomicRef atom_ref, uint32_t old,
                                         std::memory_order mo) {
  typename AtomicRef::AtomicValueType atomic_curr_value = 0;

  do {
    system::Wait(atom_ref.FutexLoc(), old);
    atomic_curr_value = atom_ref.AtomicLoad(mo);
  } while (AtomicRef::GetFutexValue(atomic_curr_value) == old);

  assert(AtomicRef::GetFutexValue(atomic_curr_value) != old);

  return atomic_curr_value;  // New value
}

}  // namespace atomic

}  // namespace futex_like
