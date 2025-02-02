#include <sure/context.hpp>

#include <cassert>
#include <cstdlib>

template <typename F>
class Coroutine : private sure::ITrampoline {
 public:
  struct SuspendContext {
    Coroutine* coro;

    void Suspend() {
      coro->Suspend();
    }
  };

 public:
  explicit Coroutine(F&& body)
      : body_(std::move(body)) {
    context_.Setup(stack_, this);
  }

  // Caller
  void Resume() {
    assert(!IsDone());
    caller_context_.SwitchTo(context_);
  }

  // Body
  void Suspend() {
    context_.SwitchTo(caller_context_);
  }

  bool IsDone() const noexcept {
    return done_;
  }

 private:
  // sure::ITrampoline
  void Run() noexcept override {
    try {
      body_(SuspendContext{this});
    } catch (...) {
      std::abort();
    }

    done_ = true;
    context_.ExitTo(caller_context_);
  }

 private:
  F body_;
  std::byte stack_[1024];
  sure::ExecutionContext context_;
  sure::ExecutionContext caller_context_;
  bool done_ = false;
};

//

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Coroutine") {
  SECTION("Just works") {
    Coroutine coro([](auto self) {
      self.Suspend();
    });

    coro.Resume();
    REQUIRE(!coro.IsDone());

    coro.Resume();
    REQUIRE(coro.IsDone());
  }

  SECTION("Interleaving") {
    Coroutine a([](auto self) {
      self.Suspend();
    });

    Coroutine b([](auto self) {
      self.Suspend();
    });

    a.Resume();
    b.Resume();

    a.Resume();
    b.Resume();

    REQUIRE(a.IsDone());
    REQUIRE(b.IsDone());
  }
}