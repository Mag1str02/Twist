#include <twist/sim.hpp>

#include <twist/assist/memory.hpp>

#include <twist/trace/scope.hpp>

#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>

static_assert(twist::build::IsolatedSim());

TEST_CASE("AssistMemoryAccess") {
  auto params = twist::sim::SimulatorParams{};
  params.crash_on_abort = false;

  SECTION("Ok") {
    auto result = twist::sim::TestSim(params, [&] {
      char* buf = new char[64];

      twist::assist::MemoryAccess(buf, 64);
      twist::assist::MemoryAccess(buf, 1);
      twist::assist::MemoryAccess(buf + 63, 1);
      twist::assist::MemoryAccess(buf + 32, 32);

      delete[] buf;
    });

    fmt::println("Stderr: {}", result.std_err);
    REQUIRE(result.Ok());
  }

  SECTION("Out of bounds #1") {
    auto result = twist::sim::TestSim(params, [&] {
      char* buf = new char[64];

      twist::assist::MemoryAccess(buf, 65);
    });

    REQUIRE(result.status == twist::sim::Status::InvalidMemoryAccess);
  }

  SECTION("Out of bounds #2") {
    auto result = twist::sim::TestSim(params, [&] {
      char* buf = new char[64];

      twist::assist::MemoryAccess(buf - 1, 3);
    });

    REQUIRE(result.status == twist::sim::Status::InvalidMemoryAccess);
  }

}

TEST_CASE("AssistPtr") {
  auto params = twist::sim::SimulatorParams{};
  params.crash_on_abort = false;

  auto result = twist::sim::TestSim(params, [&] {
    struct Widget {
      void Foo(){};
    };

    twist::assist::Ptr<Widget> w = new Widget{};
    w->Foo();

    delete w.raw;
    w->Foo();
  });

  REQUIRE(result.status == twist::sim::Status::InvalidMemoryAccess);
}

TEST_CASE("AssistNew") {
  auto params = twist::sim::SimulatorParams{};
  params.crash_on_abort = false;

  SECTION("Ok") {
    auto result = twist::sim::TestSim(params, [&] {

      struct Widget {};

      {
        twist::trace::Scope leak{"Leak"};
        new Widget{};
      }

      {
        [[maybe_unused]] auto* w = twist::assist::New<Widget>();
      }

      {
        new Widget{};
      }
    });

    // Detailed report
    fmt::println("Stderr: {}", result.std_err);

    REQUIRE(!result.Ok());
  }
}
