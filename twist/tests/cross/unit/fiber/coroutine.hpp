#pragma once

#include <sure/context.hpp>

#include <twist/assist/fiber.hpp>

#include <cstdlib>
#include <cassert>
#include <functional>

class Coroutine : private sure::ITrampoline {
  using Body = std::function<void()>;

  static constexpr size_t kStackSize = 128 * 1024;

 public:
  explicit Coroutine(Body body)
      : body_(body),
        stack_{new std::byte[kStackSize]} {

    twist::assist::ResetFiberStack({stack_, kStackSize});

    context_.Setup({stack_, kStackSize}, this);
    twist::assist::NewFiber(&fiber_, {stack_, kStackSize});
  }

  ~Coroutine() {
    delete[] stack_;
  }

  Coroutine(const Coroutine&) = delete;
  Coroutine(Coroutine&&) = delete;

  void Resume() {
    assert(!IsDone());
    caller_fiber_ = twist::assist::SwitchToFiber(fiber_.Handle());
    caller_context_.SwitchTo(context_);
  }

  void Suspend() {
    twist::assist::SwitchToFiber(caller_fiber_);
    context_.SwitchTo(caller_context_);
  }

  bool IsDone() const noexcept {
    return done_;
  }

 private:
  void Run() noexcept override {
    try {
      body_();
    } catch (...) {
      std::abort();
    }

    done_ = true;
    twist::assist::SwitchToFiber(caller_fiber_);
    context_.ExitTo(caller_context_);
  }

 private:
  Body body_;
  std::byte* stack_;

  sure::ExecutionContext context_;
  sure::ExecutionContext caller_context_;

  twist::assist::Fiber fiber_;
  twist::assist::FiberHandle caller_fiber_;

  bool done_ = false;
};
