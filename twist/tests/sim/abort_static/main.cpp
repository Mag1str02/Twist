#include <twist/sim.hpp>

#include <twist/ed/static/var.hpp>

#include <wheels/core/compiler.hpp>

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

#include <cassert>
#include <stdexcept>

static_assert(twist::build::IsolatedSim());

struct ThrowInCtor {
  ThrowInCtor() {
    throw 1;
  }
};

TWISTED_STATIC(ThrowInCtor, g);

TEST_CASE("SimStaticTimesAbort") {
  {
    twist::sim::sched::FairScheduler fair{};

    auto params = twist::sim::SimulatorParams{};
    params.crash_on_abort = false;

    twist::sim::Simulator sim{&fair, params};

    // Exception thrown from static global variable ctor

    auto result = sim.Run([] {
      WHEELS_UNREACHABLE();  // Aborted _before_ main
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::UnhandledException);
    fmt::println("Stderr: {}", result.std_err);
  }
}
