#pragma once

/*
 * Annotations for user-space fibers
 *
 * Usage:
 *
 * class Coroutine {
 *  public:
 *   Coroutine(Body body) {
 *     // Initialization
 *
 *     twist::assist::NewFiber(&fiber_ [, stack_view]);
 *   }
 *
 *   void Resume() {
 *     // NB: _Before_ context switch
 *     caller_fiber_ = twist::assist::SwitchToFiber(fiber_.Handle());
 *
 *     // Context switch
 *   }
 *
 *   void Suspend() {
 *     twist::assist::SwitchToFiber(caller_fiber_);
 *
 *     // Context switch
 *   }
 *
 *  private:
 *   // Body, stack, execution contexts, etc
 *
 *   twist::assist::Fiber fiber_;
 *   twist::assist::FiberHandle caller_fiber_;
 * };
 *
 */

#include <twist/rt/cap/assist/fiber.hpp>

namespace twist::assist {

using rt::cap::assist::StackView;
using rt::cap::assist::ResetFiberStack;

using rt::cap::assist::Fiber;
using rt::cap::assist::FiberHandle;

using rt::cap::assist::NewFiber;
using rt::cap::assist::SwitchToFiber;

}  // namespace twist::assist
