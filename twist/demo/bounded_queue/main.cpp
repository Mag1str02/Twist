#include "bounded_queue.hpp"

#include <twist/sim.hpp>

#include <twist/test/wg.hpp>

#include <wheels/core/compiler.hpp>

#include <atomic>

#include <fmt/core.h>

static_assert(twist::build::kIsolatedSim);

int main() {
  twist::sim::sched::RandomScheduler scheduler{{.seed = 42}};
  twist::sim::Simulator simulator{&scheduler};

  // Single run
  auto result = simulator.Run([] {
    // Test parameters
    static const size_t kProducers = 3;
    static const size_t kWorkers = 3;
    static const size_t kJobs = 100'000;
    static const size_t kQueueCapacity = 3;

    //

    struct Job {};

    BoundedQueue<Job> jobs{kQueueCapacity};

    std::atomic<size_t> budget{kJobs};
    std::atomic<size_t> producers{kProducers};

    twist::test::WaitGroup wg;

    // Producers

    wg.Add(kProducers, [&] {
      while (true) {
        size_t j = budget.load();
        if (j == 0) {
          break;
        } else if (budget.compare_exchange_weak(j, j - 1)) {
          jobs.Put(Job{});
        }
      }

      if (producers.fetch_sub(1) == 1) {
        // Last producer
        jobs.Close();
      }
    });

    // Workers

    wg.Add(kWorkers, [&] {
      while (auto job = jobs.Take()) {
        WHEELS_UNUSED(job);
      }
    });

    wg.Join();
  });

  if (!result.Ok()) {
    fmt::println("Stderr: {}", result.std_err);
  } else {
    fmt::println("Seems correct");
  }

  return 0;
}
