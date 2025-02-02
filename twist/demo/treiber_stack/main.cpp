#include "treiber_stack.hpp"

#include <twist/sim.hpp>

#include <twist/test/wg.hpp>

#include <twist/trace/scope.hpp>

#include <fmt/core.h>

static_assert(twist::build::kIsolatedSim);

void TestCase() {
  LockFreeStack<int> stack;

  twist::trace::Scope test;
  test.SetVisibility(-1);

  twist::test::WaitGroup{}
      .Add(/*threads=*/3, [&stack](size_t index) {
        stack.Push(index);
        stack.TryPop();
      })
      .Join();
}

int main() {
  twist::sim::sched::DfsScheduler dfs{{.max_preemptions = 5}};

  auto exp = twist::sim::Explore(dfs, TestCase);

  fmt::println("Simulations: {}", exp.sims);

  if (exp.found) {
    auto [schedule, result] = *exp.found;
    fmt::println("Stderr: {}", result.std_err);  // Corrupted memory report

    twist::sim::Print(TestCase, schedule);
  } else {
    fmt::println("Ok");
  }

  return 0;
}
