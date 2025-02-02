#pragma once

/*
 * Busy waiting
 *
 * Usage: examples/spin/main.cpp
 *
 *
 * void SpinLock::Lock() {
 *   // One-shot
 *   twist::ed::SpinWait spin_wait;
 *
 *   while (locked_.exchange(true)) {
 *     spin_wait();  // operator()() <- exponential backoff
 *   }
 * }
 *
 *
 * void Mutex::Lock() {
 *   twist::ed::SpinWait spin_wait;
 *
 *   while (!TryLock()) {
 *     if (spin_wait.ConsiderParking()) {
 *       ParkWaiter();
 *     } else {
 *       spin_wait();
 *     }
 *   }
 * }
 *
 */

#include <twist/rt/cap/wait/spin.hpp>

namespace twist::ed {

using rt::cap::SpinWait;
using rt::cap::CpuRelax;

}  // namespace twist::ed
