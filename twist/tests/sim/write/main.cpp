#include <twist/sim.hpp>

#include <twist/ed/c/write.hpp>

#include <twist/ed/fmt/print.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cassert>

static_assert(twist::build::Sim());

TEST_CASE("Write") {
  SECTION("c::write") {
    auto result = twist::sim::TestSim({}, [] {
      {
        const char* kStdOut = "Hello, world";
        const char* p = kStdOut;

        twist::ed::c::write(1, p, 7);
        twist::ed::c::write(1, p + 7, 5);
      }

      {
        const char* kStdErr = "FooBar";
        const char* p = kStdErr;

        twist::ed::c::write(2, p, 3);
        twist::ed::c::write(2, p + 3, 3);
      }
    });

    static const std::string kExpectedStdOut = "Hello, world";
    REQUIRE(result.std_out == kExpectedStdOut);

    static const std::string kExpectedStdErr = "FooBar";
    REQUIRE(result.std_err == kExpectedStdErr);
  }

  SECTION("fmt::print") {
    auto params = twist::sim::SimulatorParams{};
    params.forward_stdout = false;

    auto result = twist::sim::TestSim(params, [] {
      {
        twist::ed::fmt::print("Hello, ");
        twist::ed::fmt::println(/*fd=*/1, "world");
        twist::ed::fmt::println("{}, {}, {}", 1, 2, 3);

        twist::ed::fmt::print(2, "Error");
        twist::ed::fmt::print(2, " ");
        twist::ed::fmt::println(2, "message");
      }
    });

    static const std::string kExpectedStdOut = "Hello, world\n1, 2, 3\n";
    REQUIRE(result.std_out == kExpectedStdOut);

    static const std::string kExpectedStdErr = "Error message\n";
    REQUIRE(result.std_err == kExpectedStdErr);
  }
}
