#pragma once

#include <twist/rt/sim/system/thread/id.hpp>

#include "handle.hpp"
#include "stack.hpp"

#include <optional>

#include <twist/trace/scope.hpp>

namespace twist::rt::sim {

namespace user::fiber {

struct Fiber {
  std::optional<system::ThreadId> thread;
  uint64_t id;
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

}  // namespace user::fiber

}  // namespace twist::rt::sim
