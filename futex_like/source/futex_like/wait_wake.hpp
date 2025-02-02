#pragma once

#include "atomic/ref.hpp"
#include "atomic/wait.hpp"
#include "atomic/wait_timed.hpp"
#include "atomic/prepare_wake.hpp"
#include "atomic/wake.hpp"

namespace futex_like {

// Atomic uint64_t half refs

using atomic::AtomicUint64HighHalfRef;
using atomic::AtomicUint64LowHalfRef;

// Ref{Low,High}Half

inline auto RefLowHalf(std::atomic_uint64_t& atom) {
  return AtomicUint64LowHalfRef{atom};
}

inline auto RefHighHalf(std::atomic_uint64_t& atom) {
  return AtomicUint64HighHalfRef{atom};
}

// Wait

inline uint32_t Wait(std::atomic_uint32_t& atom, uint32_t old,
                     std::memory_order mo = std::memory_order::seq_cst) {
  return atomic::Wait(atomic::AtomicUint32Ref{atom}, old, mo);
}

template <atomic::AtomicUint64Half Half>
uint64_t Wait(atomic::AtomicUint64Ref<Half> atom_ref, uint32_t old,
              std::memory_order mo = std::memory_order::seq_cst) {
  return atomic::Wait(atom_ref, old, mo);
}

// WaitTimed

using WaitTimedUint32Status = atomic::WaitTimedStatus<uint32_t>;

inline WaitTimedUint32Status WaitTimed(
    std::atomic_uint32_t& atom, uint32_t old, std::chrono::milliseconds timeout,
    std::memory_order mo = std::memory_order::seq_cst) {
  return atomic::WaitTimed(atomic::AtomicUint32Ref{atom}, old, timeout, mo);
}

using WaitTimedUint64Status = atomic::WaitTimedStatus<uint64_t>;

template <atomic::AtomicUint64Half Half>
WaitTimedUint64Status WaitTimed(
    atomic::AtomicUint64Ref<Half> atom_ref, uint32_t old,
    std::chrono::milliseconds timeout,
    std::memory_order mo = std::memory_order::seq_cst) {
  return atomic::WaitTimed(atom_ref, old, timeout, mo);
}

// WakeKey

struct [[nodiscard]] WakeKey {
  atomic::WakeKey atom;
};

// PrepareWake

inline WakeKey PrepareWake(std::atomic_uint32_t& atom) {
  return {atomic::PrepareWake(atomic::AtomicUint32Ref{atom})};
}

template <atomic::AtomicUint64Half Half>
WakeKey PrepareWake(atomic::AtomicUint64Ref<Half> atom_ref) {
  return {atomic::PrepareWake(atom_ref)};
}

// Wake

inline void WakeOne(WakeKey key) {
  atomic::WakeOne(key.atom);
}

inline void WakeAll(WakeKey key) {
  atomic::WakeAll(key.atom);
}

}  // namespace futex_like
