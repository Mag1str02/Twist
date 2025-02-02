#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cassert>

static_assert(twist::build::Sim());

TEST_CASE("Digest") {
  SECTION("atomic<T>::load") {
    // Digest twist::ed::std::atomic<T>::load

    auto params = twist::sim::SimulatorParams{};
    params.digest_atomic_loads = true;

    auto s1 = twist::sim::TestSim(params, [] {
      twist::ed::std::atomic<int> atom{0};
      int v = atom.load();
      WHEELS_UNUSED(v);
    });

    auto s2 = twist::sim::TestSim(params, [] {
      twist::ed::std::atomic<int> atom{1};
      int v = atom.load();
      WHEELS_UNUSED(v);
    });

   REQUIRE(s1.digest != s2.digest);
  }

  SECTION("atomic<int>::fetch_add") {
    // Digest twist::ed::std::atomic<int>::fetch_add

    auto params = twist::sim::SimulatorParams{};
    params.digest_atomic_loads = true;

    auto s1 = twist::sim::TestSim(params, [] {
      twist::ed::std::atomic<int> atom{0};
      int v = atom.fetch_add(1);
      WHEELS_UNUSED(v);
    });

    auto s2 = twist::sim::TestSim(params, [] {
      twist::ed::std::atomic<int> atom{1};
      int v = atom.fetch_add(1);
      WHEELS_UNUSED(v);
    });

    REQUIRE(s1.digest != s2.digest);
  }

  SECTION("Debug{Load,Test}") {
    // twist::ed::std::atomic<T>::DebugLoad
    // twist::ed::std::atomic<T>::DebugTest

    auto params = twist::sim::SimulatorParams{};
    params.digest_atomic_loads = true;

    auto s1 = twist::sim::TestSim(params, [] {
      {
        twist::ed::std::atomic<int> atom{0};
        int v1 = atom.load();
        WHEELS_UNUSED(v1);
      }

      {
        twist::ed::std::atomic_flag f;
        bool t1 = f.test();
        WHEELS_UNUSED(t1);
      }
    });

    auto s2 = twist::sim::TestSim(params, [] {
      {
        twist::ed::std::atomic<int> atom{0};
        int v1 = atom.load();
        WHEELS_UNUSED(v1);

        for (size_t i = 0; i < 17; ++i) {
          int v2 = atom.DebugLoad();
          WHEELS_UNUSED(v2);
        }
      }

      {
        twist::ed::std::atomic_flag f;
        bool t1 = f.test();
        WHEELS_UNUSED(t1);

        for (size_t i = 0; i < 17; ++i) {
          bool t2 = f.DebugTest();
          WHEELS_UNUSED(t2);
        }
      }
    });

    // TODO: fix me
    // assert(s1.digest == s2.digest);
  }
}
