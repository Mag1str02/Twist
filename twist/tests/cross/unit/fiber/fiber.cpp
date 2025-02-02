#include <twist/cross.hpp>

#include "coroutine.hpp"

#include <twist/trace/domain.hpp>
#include <twist/trace/scope.hpp>

#include <wheels/test/framework.hpp>

class Fiber {
 public:
  Fiber(std::function<void()> fun)
      : coroutine_{fun} {
  }

 public:
  void Run() {
    self = this;
    coroutine_.Resume();
    self = nullptr;
  }

  static void Yield() {
    self->coroutine_.Suspend();
  }

  bool IsDone() const noexcept {
    return coroutine_.IsDone();
  }

  static Fiber* self;

 private:
  Coroutine coroutine_;
};

Fiber* Fiber::self = nullptr;

TEST_SUITE(Fiber) {
  SIMPLE_TEST(SwitchToFiber) {
    twist::cross::Run([] {
      Fiber f1([] {
        Fiber::Yield();
      });

      Fiber f2([] {
        Fiber::Yield();
        Fiber::Yield();
      });

      f1.Run();
      f2.Run();
      f1.Run();
      f2.Run();
      f2.Run();
    });
  }

  SIMPLE_TEST(Tracing) {
    twist::cross::Run([] {
      twist::trace::Domain test{"Test"};

      twist::trace::Scope main{test, "main"};

      Fiber fiber([&] {
        twist::trace::Scope fiber{test, "f1"};

        ASSERT_TRUE(fiber.IsRoot());

        for (size_t i = 0; i < 3; ++i) {
          twist::trace::Scope iter;

          ASSERT_TRUE(iter.GetParent() == &fiber);

          Fiber::Yield();
        }

      });

      for (size_t i = 0; i < 4; ++i) {
        twist::trace::Scope iter;
        ASSERT_TRUE(iter.GetParent() == &main);

        fiber.Run();
      }
    });
  }
}
