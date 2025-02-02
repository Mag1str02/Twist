#include <iostream>

#include <sure/context.hpp>
#include <sure/stack/mmap.hpp>

#include <chrono>
#include <iostream>

using sure::ExecutionContext;
using sure::Stack;
using sure::ITrampoline;

struct Runner : ITrampoline {
 public:
  Runner(size_t steps)
      : steps_left_(steps),
        stack_(Stack::AllocateBytes(64 * 1024)) {
    self_.Setup(stack_.MutView(), this);
  }

  size_t StepsLeft() const {
    return steps_left_;
  }

  void MakeStep() {
    parent_.SwitchTo(self_);
  }

  // Trampoline
  void Run() noexcept {
    while (steps_left_ > 1) {
      --steps_left_;
      self_.SwitchTo(parent_);
    }

    --steps_left_;
    self_.ExitTo(parent_);
  }

 private:
  size_t steps_left_;
  Stack stack_;
  ExecutionContext self_;
  ExecutionContext parent_;
};

int main() {
  static const size_t kSteps = 100'000'000;

  auto start = std::chrono::steady_clock::now();

  Runner runner{kSteps};
  while (runner.StepsLeft() > 0) {
    runner.MakeStep();
  }

  std::chrono::nanoseconds elapsed = std::chrono::steady_clock::now() - start;

  std::cout << "Nanos per step = " << (1.0 * elapsed.count()) / kSteps << std::endl;

  return 0;
}
