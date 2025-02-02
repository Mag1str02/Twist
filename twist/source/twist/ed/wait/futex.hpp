#pragma once

/*
 * Replacement for std::atomic<T>::wait/notify
 * std::atomic<T>::wait/notify api & impl are fundamentally broken
 *
 * Contents
 *   namespace twist::ed::futex
 *     fun Wait
 *     fun WaitTimed
 *     class WakeKey
 *     fun PrepareWake
 *     fun WakeOne
 *     fun WakeAll
 *     fun RefLowHalf
 *     fun RefHighHalf
 *     class AtomicUint64LowHalfRef
 *     class AtomicUint64HighHalfRef
 *
 * Usage: examples/futex/main.cpp
 *
 */

#include <twist/rt/cap/wait/futex.hpp>

namespace twist::ed {

namespace futex {

using rt::cap::futex::RefLowHalf;
using rt::cap::futex::RefHighHalf;

using rt::cap::futex::AtomicUint64LowHalfRef;
using rt::cap::futex::AtomicUint64HighHalfRef;

using rt::cap::futex::Wait;
using rt::cap::futex::WaitTimed;

using rt::cap::futex::WakeKey;

using rt::cap::futex::PrepareWake;

using rt::cap::futex::WakeOne;
using rt::cap::futex::WakeAll;

}  // namespace futex

}  // namespace twist::ed
