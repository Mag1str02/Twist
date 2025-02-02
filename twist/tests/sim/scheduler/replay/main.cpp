#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/wait/futex.hpp>

#include <twist/test/wg.hpp>

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

static_assert(twist::build::IsolatedSim());

TEST_CASE("ReplayScheduler") {
  SECTION("Coop") {
    auto main = [] {
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
    };

    auto [replay, coop] = [main]{
      twist::sim::sched::CoopScheduler scheduler{};
      twist::sim::sched::Recorder recorder{&scheduler};
      twist::sim::Simulator sim{&recorder};

      auto result = sim.Run(main);

      REQUIRE(!result.Ok());

      fmt::println("Stderr = {}", result.std_err);

      return std::pair{recorder.GetSchedule(), result};
    }();

    {
      twist::sim::sched::ReplayScheduler scheduler{replay};
      twist::sim::Simulator sim{&scheduler};

      auto result = sim.Run(main);

      fmt::println("Replay stderr = {}", result.std_err);

      REQUIRE(coop.digest == result.digest);
    }
  }

  SECTION("Random") {
    // Replay random

    class Mutex {
     public:
      void Lock() {
        while (locked_.exchange(1, std::memory_order::acquire) == 1) {
          twist::ed::futex::Wait(locked_, 1, std::memory_order::relaxed);
        }
      }

      void Unlock() {
        auto wake_key = twist::ed::futex::PrepareWake(locked_);
        locked_.store(0, std::memory_order::release);
        twist::ed::futex::WakeOne(wake_key);
      }

     private:
      twist::ed::std::atomic<uint32_t> locked_{0};
    };

    auto main = [] {
      Mutex mutex;

      twist::test::WaitGroup{}
        .Add(3, [&] {
            for (size_t i = 0; i < 256; ++i) {
              mutex.Lock();
              mutex.Unlock();
            }
          })
        .Join();
    };

    auto [replay, random] = [main]{
      twist::sim::sched::RandomScheduler scheduler{};
      twist::sim::sched::Recorder recorder{&scheduler};
      twist::sim::Simulator sim{&recorder};

      auto result = sim.Run(main);

      fmt::println("Stderr = {}", result.std_err);

      return std::pair{recorder.GetSchedule(), result};
    }();

    {
      twist::sim::sched::ReplayScheduler scheduler{replay};
      twist::sim::Simulator sim{&scheduler};

      auto result = sim.Run(main);

      fmt::println("Replay stderr = {}", result.std_err);

      REQUIRE(random.digest == result.digest);
    }
  }
}
