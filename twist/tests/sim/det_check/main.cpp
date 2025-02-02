#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>

#include <twist/ed/c/abort.hpp>

#include <twist/assist/assert.hpp>

#include <wheels/core/compiler.hpp>

#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>

#include <random>
#include <stdexcept>

static_assert(twist::build::IsolatedSim());

TEST_CASE("Determinism checking") {
  // Non-deterministic

  SECTION("std::random_device") {
    bool det = twist::sim::DetCheck([] {
      std::random_device d;

      for (size_t i = 0; i < 256; ++i) {
        if (d() % 2 == 1) {
          twist::ed::std::this_thread::yield();
        }
      }
    });

    REQUIRE(!det);
  }

  SECTION("External state") {
    int iter = 0;

    bool det = twist::sim::DetCheck([&] {
      twist::ed::std::atomic<int> atom{0};
      atom.store(++iter);
      atom.load();
    });

    REQUIRE(!det);
  }

  SECTION("Perpetual") {
    bool det = twist::sim::DetCheck([] {
      while (true) {
        twist::ed::std::this_thread::yield();
      }
    });

    REQUIRE(det);
  }

  // Crashing

  SECTION("twist::ed::system::Abort") {
    bool det = twist::sim::DetCheck([] {
      twist::ed::c::abort();

      WHEELS_UNREACHABLE();
    });

    REQUIRE(det);
  }

  SECTION("Unhandled exception") {
    bool det = twist::sim::DetCheck([] {
      throw std::runtime_error("Test");

      WHEELS_UNREACHABLE();
    });

    REQUIRE(det);
  }

  SECTION("TWIST_ASSERT") {
    bool det = twist::sim::DetCheck([] {
      TWIST_ASSERT(2 + 2 == 5, "Arithmetic failure");

      WHEELS_UNREACHABLE();
    });

    REQUIRE(det);
  }

  {
    // mutex

    SECTION("mutex::unlock") {
      // unlock

      bool det = twist::sim::DetCheck([] {
        twist::ed::std::mutex mu;

        mu.unlock();

        WHEELS_UNREACHABLE();
      });

      REQUIRE(det);
    }

    SECTION("mutex owner") {
      // mutex owner

      bool det = twist::sim::DetCheck([] {
        twist::ed::std::mutex mu;

        mu.lock();

        twist::ed::std::thread t([&] {
          mu.unlock();  // <- Aborted
        });

        t.join();

        WHEELS_UNREACHABLE();
      });

      REQUIRE(det);
    }
  }

  {
    // Memory

    SECTION("Double-free") {
      bool det = twist::sim::DetCheck([] {
        int* p = new int{17};
        delete p;
        delete p;  // <- Aborted

        WHEELS_UNREACHABLE();
      });

      REQUIRE(det);
    }

    SECTION("Memory leak") {
      bool det = twist::sim::DetCheck([] {
        new int{1};
      });

      REQUIRE(det);
    }
  }

  SECTION("Deadlock") {
    bool det = twist::sim::DetCheck([] {
      twist::ed::std::mutex mu;

      mu.lock();
      mu.lock();
    });

    REQUIRE(det);
  }
}
