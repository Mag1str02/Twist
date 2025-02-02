#include <twist/sim.hpp>

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/fmt/print.hpp>

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

static_assert(twist::build::IsolatedSim());

TEST_CASE("CoopScheduler") {
  SECTION("Deadlock #1") {
    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::ed::std::mutex mu1;
      twist::ed::std::mutex mu2;

      twist::ed::std::thread t1([&] {
        mu1.lock();
        twist::ed::std::this_thread::yield();  // <- 2nd switch
        mu2.lock();
      });

      twist::ed::std::thread t2([&] {
        mu2.lock();
        mu1.lock();  // <- 3rd switch
      });

      t1.join();  // <- 1st switch
      t2.join();
    });

    REQUIRE(!result.Ok());

    fmt::println("Stderr = {}", result.std_err);
  }

  SECTION("Deadlock #2") {
    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::ed::std::thread t2([] {
        twist::ed::fmt::print("2");
        twist::sim::SwitchTo(3);
      });

      twist::ed::std::thread t3([] {
        twist::ed::fmt::print("3");
        twist::sim::SwitchTo(2);
      });

      twist::ed::std::thread t4([] {
        twist::ed::fmt::print("4");
        twist::sim::SwitchTo(2);
      });

      twist::ed::fmt::print("1");

      twist::sim::SwitchTo(4);

      t2.join();
      t3.join();
      t4.join();
    });

    REQUIRE(result.Ok());
    REQUIRE(result.std_out == "1423");

    fmt::println("Stdout = {}", result.std_out);
  }
}
