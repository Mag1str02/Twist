#pragma once

#include <twist/rt/cap/test/budget.hpp>

/*
 * Usage:
 *
 * Mutex mu;
 *
 * twist::test::WaitGroup wg;
 * twist::test::Budget budget{5};
 *
 * wg.Add(3, [&] {
 *   while (budget.Withdraw(1)) {
 *     mu.Lock();
 *     mu.Unlock();
 *   }
 * });
 *
 * wg.Join();
 *
 */

namespace twist::test {

using rt::cap::test::Budget;

}  // namespace twist::test
