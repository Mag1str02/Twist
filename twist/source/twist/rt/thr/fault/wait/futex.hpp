#pragma once

#include "atomic_ref/uint32.hpp"
#include "atomic_ref/uint64.hpp"

#include <futex_like/wait_wake.hpp>

#include <cstdint>

namespace twist::rt::thr::fault {

namespace futex {

// Atomic64{Low,High}HalfRef

using AtomicUint64LowHalfRef = FaultyAtomicUint64Ref<AtomicUint64Half::Low>;
using AtomicUint64HighHalfRef = FaultyAtomicUint64Ref<AtomicUint64Half::High>;

// Ref{Low,High}Half

inline auto RefLowHalf(FaultyAtomicUint64& atomic) {
  return AtomicUint64LowHalfRef{atomic};
}

inline auto RefHighHalf(FaultyAtomicUint64& atomic) {
  return AtomicUint64HighHalfRef{atomic};
}

// Wait

inline auto Wait(FaultyAtomicUint32& atomic, uint32_t old,
                 std::memory_order mo = std::memory_order::seq_cst) {
  return futex_like::atomic::Wait(FaultyAtomicUint32Ref{atomic}, old, mo);
}

template <AtomicUint64Half Half>
auto Wait(FaultyAtomicUint64Ref<Half> atomic_ref, uint32_t old,
          std::memory_order mo = std::memory_order::seq_cst) {
  return futex_like::atomic::Wait(atomic_ref, old, mo);
}

// WaitTimed

inline auto WaitTimed(FaultyAtomicUint32& atomic, uint32_t old,
                      std::chrono::milliseconds timeout,
                      std::memory_order mo = std::memory_order::seq_cst) {
  return futex_like::atomic::WaitTimed(FaultyAtomicUint32Ref{atomic}, old, timeout, mo);
}

template <AtomicUint64Half Half>
auto WaitTimed(FaultyAtomicUint64Ref<Half> atomic_ref, uint32_t old,
               std::chrono::milliseconds timeout,
               std::memory_order mo = std::memory_order::seq_cst) {
  return futex_like::atomic::WaitTimed(atomic_ref, old, timeout, mo);
}

// WakeKey

struct WakeKey {
  futex_like::atomic::WakeKey atom;
};

// PrepareWake

inline WakeKey PrepareWake(FaultyAtomicUint32& atomic) {
  return {futex_like::atomic::PrepareWake(FaultyAtomicUint32Ref{atomic})};
}

template <AtomicUint64Half Half>
WakeKey PrepareWake(FaultyAtomicUint64Ref<Half> atomic_ref) {
  return {futex_like::atomic::PrepareWake(atomic_ref)};
}

// Wake

inline void WakeOne(WakeKey key) {
  futex_like::atomic::WakeOne(key.atom);
}

inline void WakeAll(WakeKey key) {
  futex_like::atomic::WakeAll(key.atom);
}

}  // namespace futex

}  // namespace twist::rt::thr::fault
