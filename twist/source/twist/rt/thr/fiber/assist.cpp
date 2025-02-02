#include "assist.hpp"

namespace twist::rt::thr {

namespace fiber {

// Thread-local state

static thread_local Fiber this_thread_fiber;

static thread_local Fiber* curr_fiber = &this_thread_fiber;

// Accessors

void ResetStack(StackView /*stack*/) {
  // TODO
}

void NewFiber(Fiber*) {
}

void NewFiber(Fiber* fiber, StackView stack) {
  fiber->stack = stack;
}

Fiber* SwitchToFiber(Fiber* target) {
  Fiber* curr = curr_fiber;
  curr_fiber = target;
  return curr;
}

Fiber* CurrentFiber() {
  return curr_fiber;
}

}  // namespace fiber

}  // namespace twist::rt::thr
