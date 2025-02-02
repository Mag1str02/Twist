#pragma once

#include "handle.hpp"
#include "stack.hpp"

#include <twist/trace/scope.hpp>

namespace twist::rt::thr {

namespace fiber {

struct Fiber {
  StackView stack;
  trace::Scope* scope = nullptr;

  Fiber() {
    //
  }

  Fiber(const Fiber&) = delete;
  Fiber& operator=(const Fiber&) = delete;

  Fiber(Fiber&&) = delete;
  Fiber& operator=(Fiber&&) = delete;

  FiberHandle Handle() {
    return {this};
  }
};

}  // namespace fiber

}  // namespace twist::rt::thr
