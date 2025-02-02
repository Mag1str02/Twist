#pragma once

#include <twist/rt/cap/test/wait_group.hpp>

/*
 * Usage:
 *
 * Mutex mu;
 *
 * twist::test::WaitGroup wg;
 *
 * // Run 3 threads concurrently
 * wg.Add(3, [&] {
 *   mu.Lock();
 *   mu.Unlock();
 * });
 *
 * wg.Join();
 *
 *
 */

namespace twist::test {

using rt::cap::test::WaitGroup;

}  // namespace twist::test
