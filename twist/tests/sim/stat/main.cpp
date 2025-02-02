#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/wait/futex.hpp>

#include <twist/assist/assert.hpp>

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

using namespace std::chrono_literals;

static_assert(twist::build::Sim());

TEST_CASE("Stat") {
  SECTION("Futex") {
    twist::sim::sched::CoopScheduler scheduler{};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      assert(twist::sim::stat::FutexWaitSystemCallCount() == 0);
      assert(twist::sim::stat::FutexWakeSystemCallCount() == 0);

      int data = 0;
      twist::ed::std::atomic<uint32_t> flag = 0;

      twist::ed::std::thread t([&] {
        auto wake_key = twist::ed::futex::PrepareWake(flag);
        data = 42;
        flag.store(true);
        twist::ed::futex::WakeOne(wake_key);
      });

      twist::ed::futex::Wait(flag, 0);

      TWIST_ASSERT(data == 42, "Missing producer");

      assert(twist::sim::stat::FutexWaitSystemCallCount() == 1);
      assert(twist::sim::stat::FutexWakeSystemCallCount() == 2);

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("Memory") {
    twist::sim::sched::CoopScheduler scheduler{};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      size_t ws1 = twist::sim::stat::WorkingSetInBytes();
      size_t ac1 = twist::sim::stat::AllocationCount();

      int64_t* cell = new int64_t{};

      size_t ws2 = twist::sim::stat::WorkingSetInBytes();
      size_t ac2 = twist::sim::stat::AllocationCount();

      assert(ac2 == ac1 + 1);
      assert(ws2 == ws1 + 8);

      char* buf = new char[1024];

      size_t ws3 = twist::sim::stat::WorkingSetInBytes();
      size_t ac3 = twist::sim::stat::AllocationCount();

      assert(ac3 == ac1 + 2);
      assert(ws3 == ws2 + 1024);

      delete cell;
      delete[] buf;
    });

    REQUIRE(result.Ok());
  }

  SECTION("Atomic") {
    twist::sim::sched::CoopScheduler scheduler{};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      assert(twist::sim::stat::AtomicCount() == 0);

      twist::ed::std::atomic_uint32_t atom1{5};

      assert(twist::sim::stat::AtomicCount() == 1);

      twist::ed::std::atomic_uint64_t atom2{7};

      assert(twist::sim::stat::AtomicCount() == 2);
    });

    REQUIRE(result.Ok());
  }
}
