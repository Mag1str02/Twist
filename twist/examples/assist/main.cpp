#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/fmt/print.hpp>

#include <twist/assist/assert.hpp>
#include <twist/assist/shared.hpp>
#include <twist/assist/memory.hpp>
#include <twist/assist/random.hpp>

#include <fmt/core.h>

#include <functional>

static_assert(twist::build::IsolatedSim());

void AssertExample() {
  TWIST_ASSERT(2 + 2 == 5, "Arithmetic failure");
}

void SharedExample() {
  twist::assist::Shared<int> data{0};
  twist::ed::std::atomic<bool> flag{false};

  twist::ed::std::thread t2([&] {
    *data = 42;
    flag.store(true, std::memory_order::release);
  });

  twist::sim::SwitchTo(2);  // Assuming CoopScheduler

  if (flag.load(std::memory_order::relaxed)) {
    int d = data;  // <- Data race
    twist::ed::fmt::println("data = {}", d);
  }

  t2.join();
}

void MemoryPtrExample() {
  struct Widget {
    int data;
  };

  Widget* w = new Widget{42};

  twist::assist::Ptr<Widget> wd = w;

  delete w;

  int d = wd->data;  // <- Use-after-free
  twist::ed::fmt::println("data = {}", d);
}

void MemoryAccessExample() {
  char* buf = new char[64];

  twist::assist::MemoryAccess(buf + 7, 15);
  twist::assist::MemoryAccess(buf + 63, 2);  // <- Out-of-bounds

  delete[] buf;
}

void Example(std::function<void()> main) {
  twist::sim::sched::CoopScheduler coop{};
  twist::sim::Simulator sim{&coop};
  auto result = sim.Run(main);
  fmt::println("Status: {}, stderr: {}", result.status, result.std_err);
}

int main() {
  Example(AssertExample);
  Example(SharedExample);
  Example(MemoryPtrExample);
  Example(MemoryAccessExample);

  return 0;
}
