#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cassert>

using namespace std::chrono_literals;

static_assert(twist::build::IsolatedSim());

TEST_CASE("thread::detach") {
  SECTION("yield") {
    twist::sim::sched::FairScheduler fair{};
    twist::sim::Simulator sim{&fair};

    auto result = sim.Run([] {
      twist::ed::std::atomic<size_t> iter{0};

      twist::ed::std::thread t([&] {
        while (true) {
          ++iter;
          twist::ed::std::this_thread::yield();
        }
      });

      t.detach();
      assert(!t.joinable());

      while (iter < 7) {
        twist::ed::std::this_thread::sleep_for(1ms);
      }

      // <- t is running
    });

    REQUIRE(result.Ok());
  }

  SECTION("sleep_for") {
    twist::sim::sched::FairScheduler fair{};
    twist::sim::Simulator sim{&fair};

    auto result = sim.Run([] {
      twist::ed::std::atomic<size_t> iter{0};

      twist::ed::std::thread t([&] {
        while (true) {
          ++iter;
          twist::ed::std::this_thread::sleep_for(1s);
        }
      });

      t.detach();

      while (iter < 7) {
        twist::ed::std::this_thread::sleep_for(1s);
      }

      // <- t is running
    });

    REQUIRE(result.Ok());
  }
}
