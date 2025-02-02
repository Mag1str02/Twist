#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/shared_mutex.hpp>
#include <twist/ed/std/thread.hpp>

#include <twist/ed/c/abort.hpp>

#include <twist/assist/assert.hpp>
#include <twist/assist/memory.hpp>
#include <twist/assist/shared.hpp>

#include <wheels/core/compiler.hpp>

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

#include <cassert>
#include <stdexcept>

static_assert(twist::build::IsolatedSim());

using SimStatus = twist::sim::Status;

TEST_CASE("SimAbort") {
  auto params = twist::sim::Simulator::Params{};
  params.crash_on_abort = false;

  SECTION("User abort") {
    // twist::ed::c::abort()

    auto result = twist::sim::TestSim(params, [] {
      twist::ed::c::abort();

      WHEELS_UNREACHABLE();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::UserAbort);
  }

  SECTION("Unhandled exception in main thread") {
    auto result = twist::sim::TestSim(params, [] {
      throw std::runtime_error("Unhandled");

      WHEELS_UNREACHABLE();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::UnhandledException);
    fmt::println("Stderr: {}", result.std_err);
  }

  SECTION("Unhandled exception in user thread") {
    auto result = twist::sim::TestSim(params, [] {
      twist::ed::std::thread t([] {
        throw std::runtime_error("Unhandled");
      });

      t.join();

      WHEELS_UNREACHABLE();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::UnhandledException);
    fmt::println("Stderr: {}", result.std_err);
  }

  SECTION("twist::assist::Panic") {
    auto result = twist::sim::TestSim(params, [] {
      twist::assist::Panic("Test");
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::UserAbort);
    fmt::println("Stderr: {}", result.std_err);
  }

#if !defined(NDEBUG)
  SECTION("TWIST_ASSERT") {
    auto result = twist::sim::TestSim(params, [] {
      TWIST_ASSERT(2 + 2 == 5, "Arithmetic failure");

      WHEELS_UNREACHABLE();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::UserAbort);
    fmt::println("Stderr: {}", result.std_err);
  }

#endif

  SECTION("TWIST_ASSERT") {
    auto result = twist::sim::TestSim(params, [] {
      TWIST_ASSERT(2 + 2 == 5, "Arithmetic failure");

      WHEELS_UNREACHABLE();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::UserAbort);
    fmt::println("Stderr: {}", result.std_err);
  }

  {
    // mutex

    SECTION("mutex::unlock") {
      auto result = twist::sim::TestSim(params, [] {
        twist::ed::std::mutex mu;

        mu.unlock();

        WHEELS_UNREACHABLE();
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::LibraryAssert);
      fmt::println("Stderr: {}", result.std_err);
    }

    SECTION("mutex owner") {
      auto result = twist::sim::TestSim(params, [] {
        twist::ed::std::mutex mu;

        mu.lock();

        twist::ed::std::thread t([&] {
          mu.unlock();  // <- Aborted
        });

        t.join();

        WHEELS_UNREACHABLE();
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::LibraryAssert);
      fmt::println("Stderr: {}", result.std_err);
    }
  }

  {
    // shared_mutex

    SECTION("shared_mutex::unlock") {
      // unlock

      auto result = twist::sim::TestSim(params, [] {
        twist::ed::std::shared_mutex mu;

        mu.unlock();

        WHEELS_UNREACHABLE();
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::LibraryAssert);
      fmt::println("Stderr: {}", result.std_err);
    }

    SECTION("shared_mutex::unlock_shared") {
      auto result = twist::sim::TestSim(params, [] {
        twist::ed::std::shared_mutex mu;

        mu.unlock_shared();

        WHEELS_UNREACHABLE();
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::LibraryAssert);
      fmt::println("Stderr: {}", result.std_err);
    }
  }

  {
    // thread

    SECTION("Missing thread::join") {
      auto result = twist::sim::TestSim(params, [] {
        twist::ed::std::thread t([] {});
        // Aborted in thread dtor
      });

      REQUIRE(!result.Ok());
      fmt::println("Stderr: {}", result.std_err);
    }

    SECTION("thread::join after detach") {
      auto result = twist::sim::TestSim(params, [] {
        twist::ed::std::thread t([] {});
        t.detach();
        t.join();  // <- Aborted

        WHEELS_UNREACHABLE();
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::LibraryAssert);
      fmt::println("Stderr: {}", result.std_err);
    }
  }

  {
    // Memory

    SECTION("Double-free") {
      // Double-free

      auto result = twist::sim::TestSim(params, [] {
      int* p = new int{17};
      delete p;
      delete p;  // <- Aborted

      WHEELS_UNREACHABLE();
    });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::MemoryDoubleFree);
      fmt::println("Stderr: {}", result.std_err);
    }

    SECTION("Memory leak") {
      // Memory leak

      auto result = twist::sim::TestSim(params, [] {
        new int{1};
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::MemoryLeak);
      fmt::println("Stderr: {}", result.std_err);
    }

    SECTION("Heap-use-after-free") {
      // Heap-use-after-free

      auto result = twist::sim::TestSim(params, [] {
        struct Node {
          int data;
        };

        Node* n = new Node{7};
        [[maybe_unused]] int d1 = twist::assist::Ptr(n)->data;
        delete n;

        [[maybe_unused]] int d2 = twist::assist::Ptr(n)->data;

        WHEELS_UNREACHABLE();
      });

      REQUIRE(!result.Ok());
      REQUIRE(result.status == SimStatus::InvalidMemoryAccess);
      fmt::println("Stderr: {}", result.std_err);
    }
  }

  SECTION("Deadlock #1") {
    auto result = twist::sim::TestSim(params, [] {
      twist::ed::std::mutex mu;

      mu.lock();
      mu.lock();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::Deadlock);
    // fmt::println("Stderr: {}", result.std_err);
  }

  SECTION("Deadlock #2") {
    auto result = twist::sim::TestSim(params, [] {
      while (true) {
        twist::ed::std::mutex mu;

        mu.lock();

        twist::ed::std::thread t([&] {
          mu.lock();  // <- Deadlocked
          mu.unlock();
        });

        t.join();  // <- Deadlocked
        mu.unlock();

        t.join();
      }
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::Deadlock);

    // fmt::println("Stderr: {}", result.std_err);
  }

  SECTION("Data race") {
    // Data race

    twist::sim::sched::CoopScheduler scheduler{{42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> x{0};

      twist::ed::std::thread t([&] {
        x.Write(2);
      });

      x.Write(1);

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::DataRace);

    fmt::println("Stderr: {}", result.std_err);
  }

  SECTION("Dead atomic") {
    // Dead atomic access

    twist::sim::sched::CoopScheduler scheduler{{42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      auto* atom = new twist::ed::std::atomic_int64_t{7};

      delete atom;

      atom->store(1);
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == SimStatus::InvalidMemoryAccess);

    fmt::println("Stderr: {}", result.std_err);
  }
}
