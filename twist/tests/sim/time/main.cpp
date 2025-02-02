#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/chrono.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cassert>

using namespace std::chrono_literals;

static_assert(twist::build::Sim());

auto SimSteadyNow() {
  return twist::ed::std::chrono::steady_clock::now();
}

TEST_CASE("Time") {
  SECTION("Compression") {
    twist::sim::sched::FairScheduler fair{};
    twist::sim::Simulator sim{&fair};

    auto result = sim.Run([] {
      static const std::chrono::seconds kPause = 1'000'000s;

      auto start_time = SimSteadyNow();

      twist::ed::std::this_thread::sleep_for(kPause);

      auto end_time = SimSteadyNow();
      assert(end_time >= start_time);

      auto elapsed = end_time - start_time;

      assert(elapsed >= kPause);
    });

    REQUIRE(result.Ok());
  }

  SECTION("Granularity") {
    twist::sim::sched::FairScheduler fair{};
    auto params = twist::sim::SimulatorParams{.tick = 1ms};
    twist::sim::Simulator sim{&fair, params};

    auto result = sim.Run([] {
      auto start_time = SimSteadyNow();

      twist::ed::std::this_thread::sleep_for(1us);
      auto end_time = SimSteadyNow();

      auto elapsed = end_time - start_time;
      assert(elapsed >= 1ms);
    });

    REQUIRE(result.Ok());
  }

  SECTION("fetch_add") {
    twist::sim::sched::FairScheduler fair{};
    auto params = twist::sim::SimulatorParams{.tick = 1ms};
    twist::sim::Simulator sim{&fair, params};

    auto result = sim.Run([] {
      twist::ed::std::atomic<int> atom{0};

      auto last = SimSteadyNow();
      for (size_t i = 0; i < 10; ++i) {
        atom.fetch_add(1);

        auto now = SimSteadyNow();
        assert(now > last);
        last = now;
      }
    });

    REQUIRE(result.Ok());
  }

  SECTION("yield") {
    twist::sim::sched::FairScheduler fair{};
    auto params = twist::sim::SimulatorParams{.tick = 1us};
    twist::sim::Simulator sim{&fair, params};

    auto result = sim.Run([] {
      auto start_time = SimSteadyNow();

      static const size_t kIters = 17;

      for (size_t i = 0; i < kIters; ++i) {
        twist::ed::std::this_thread::yield();
      }

      auto end_time = SimSteadyNow();
      assert(end_time >= start_time);

      auto elapsed = end_time - start_time;
      assert(elapsed == kIters * 1us);
    });

    REQUIRE(result.Ok());
  }
}
