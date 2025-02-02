#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/assist/assert.hpp>
#include <twist/test/wg.hpp>

#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>

static_assert(twist::build::IsolatedSim());

void Sim(size_t threads, size_t incrs_per_thread) {
  twist::ed::std::atomic<size_t> atom{0};

  twist::test::WaitGroup{}
      .Add(threads,
           [&] {
             for (size_t i = 0; i < incrs_per_thread; ++i) {
               atom.fetch_add(1);
             }
           })
      .Join();

  TWIST_ASSERT(atom.load() == threads * incrs_per_thread,
                 "Non-atomic RMW");
}

TEST_CASE("RMW") {
  SECTION("RandomScheduler") {
    twist::sim::sched::RandomScheduler scheduler{{}};

    for (size_t i = 0; i < 64; ++i) {
      twist::sim::Simulator simulator{&scheduler};
      auto result = simulator.Run([] { Sim(5, 17); });
      REQUIRE(result.Ok());

      scheduler.NextSchedule();
    }
  }

  SECTION("PctScheduler") {
    twist::sim::sched::PctScheduler scheduler{{.depth=5}};

    for (size_t i = 0; i < 64; ++i) {
      twist::sim::Simulator simulator{&scheduler};
      auto result = simulator.Run([] { Sim(5, 17); });
      REQUIRE(result.Ok());

      scheduler.NextSchedule();
    }
  }

  SECTION("DfsScheduler") {
    auto params = twist::sim::sched::DfsScheduler::Params{};
    params.max_preemptions = 6;

    twist::sim::sched::DfsScheduler scheduler{{}};

    do {
      auto sim_params = twist::sim::SimulatorParams{};
      sim_params.randomize_malloc = false;

      twist::sim::Simulator simulator{&scheduler, sim_params};
      auto result = simulator.Run([] { Sim(3, 2); });
      REQUIRE(result.Ok());
    } while (scheduler.NextSchedule());
  }
}
