#pragma once

#include <twist/rt/thr/fiber/assist.hpp>

namespace twist::rt::thr {

namespace assist {

using fiber::StackView;
using fiber::Fiber;
using fiber::FiberHandle;

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

}  // namespace assist

}  // namespace twist::rt::thr
