#pragma once

#include <twist/rt/cap/test/lock_free.hpp>

/*
 * Non-blocking synchronization
 *
 * {
 *   LockFreeScope lf;
 *
 *   // Push-pop from lock-free stack
 * }
 *
 * // Hint to scheduler
 * void Progress();
 *
 */

namespace twist::test {

using rt::cap::test::SetThreadAttrLockFree;
using rt::cap::test::LockFreeScope;

using rt::cap::test::Progress;

}  // namespace twist::test
