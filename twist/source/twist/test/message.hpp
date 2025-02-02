#pragma once

#include <twist/rt/cap/test/message.hpp>

/*
 * Immutable message for testing message passing scenarios
 *
 * LockFreeStack<Message<int>> stack;
 *
 * // Producer
 * stack.Push(twist::test::Message<int>::New(42));
 *
 * // Consumer
 * if (auto message = stack.TryPop()) {
 *   message.Read();  // Happens-before checking (via Twist or ThreadSanitizer)
 * }
 *
 */

namespace twist::test {

using rt::cap::test::Message;

}  // namespace twist::test
