#include <twist/sim.hpp>

#include <twist/ed/std/thread.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cassert>
#include <random>

using namespace std::chrono_literals;

static_assert(twist::build::IsolatedSim());

TEST_CASE("Params") {
  auto params = twist::sim::SimulatorParams{};
  params.hardware_concurrency = 48;

  twist::sim::TestSim(params, [] {
    size_t hw = twist::ed::std::thread::hardware_concurrency();
    assert(hw == 48);
  });
}
