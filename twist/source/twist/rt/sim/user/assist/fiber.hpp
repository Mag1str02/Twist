#pragma once

#include <twist/rt/sim/user/fiber/assist.hpp>

namespace twist::rt::sim {

namespace user::assist {

using user::fiber::StackView;
using user::fiber::Fiber;
using user::fiber::FiberHandle;

inline void ResetFiberStack(StackView stack) {
  fiber::ResetStack(stack);
}

inline void NewFiber(Fiber* fiber) {
  fiber::NewFiber(fiber);
}

inline void NewFiber(Fiber* fiber, StackView stack) {
  fiber::NewFiber(fiber, stack);
}

inline FiberHandle SwitchToFiber(FiberHandle target) {
  return {fiber::SwitchToFiber(target.fiber)};
}

}  // namespace user::assist

}  // namespace twist::rt::sim
