#include <twist/sim.hpp>

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>

#include <fmt/core.h>

static_assert(twist::build::kIsolatedSim);

void MutexLockOrder() {
  // main ~ t1

  twist::ed::std::mutex a;
  twist::ed::std::mutex b;

  twist::ed::std::thread t2([&] {
    a.lock();
    b.lock();
    b.unlock();
    a.unlock();
  });

  twist::ed::std::thread t3([&] {
    b.lock();
    a.lock();
    a.unlock();
    b.unlock();
  });

  t2.join();
  t3.join();
}

int main() {
  twist::sim::sched::DfsScheduler dfs{{.max_preemptions = 3}};

  // Exhaustive search over all schedules
  auto exp = twist::sim::Explore(dfs, MutexLockOrder);

  assert(exp.found);

  {
    // Schedule, simulation result
    auto [schedule, result] = *exp.found;

    assert(result.status == twist::sim::Status::Deadlock);

    twist::sim::Print(MutexLockOrder, schedule);
    fmt::println("Stderr: {}", result.std_err);  // Deadlock report
  }

  return 0;
}
