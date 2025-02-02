#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>

#include <twist/ed/wait/spin.hpp>

#include <twist/ed/c/abort.hpp>

#include <twist/assist/random.hpp>
#include <twist/assist/assert.hpp>
#include <twist/assist/memory.hpp>
#include <twist/assist/preempt.hpp>
#include <twist/assist/prune.hpp>

#include <twist/test/wg.hpp>
#include <twist/test/either.hpp>

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

static_assert(twist::build::IsolatedSim());

struct ExploreResult {
  std::optional<twist::sim::Result> crash;
  size_t count = 0;
};

ExploreResult Explore(twist::sim::sched::DfsScheduler::Params dfs_params, std::function<void()> main) {
  ExploreResult result{};

  auto sim_params = twist::sim::SimulatorParams{};
  sim_params.forward_stdout = false;
  sim_params.crash_on_abort = false;
  sim_params.randomize_malloc = false;

  twist::sim::sched::DfsScheduler scheduler{dfs_params};
  do {
    twist::sim::Simulator sim{&scheduler, sim_params};

    ++result.count;
    auto sim_result = sim.Run([main] { main(); });

    if (!sim_result.Ok() && sim_result.Complete() && !result.crash) {
      result.crash = sim_result;
      // break;
    }
  } while (scheduler.NextSchedule());

  return result;
}

TEST_CASE("DfsScheduler") {
  SECTION("Deadlock") {
    // Deadlock
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      twist::ed::std::mutex mu1;
      twist::ed::std::mutex mu2;

      twist::ed::std::thread t1([&] {
        mu1.lock();
        mu2.lock();
        mu2.unlock();
        mu1.unlock();
      });

      twist::ed::std::thread t2([&] {
        mu2.lock();
        mu1.lock();
        mu1.unlock();
        mu2.unlock();
      });

      t1.join();
      t2.join();
    });

    REQUIRE(result.crash);

    auto crash = *result.crash;
    REQUIRE(crash.status == twist::sim::Status::Deadlock);
    fmt::println("Deadlock stderr: {}", crash.std_err);
  }

  SECTION("max_steps") {
    auto params = twist::sim::sched::DfsScheduler::Params{};
    params.max_steps = 17;

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      while (true) {
        atom.fetch_add(1);
      }
    });

    REQUIRE(!result.crash);
    REQUIRE(result.count == 1);
  }

  SECTION("Prune") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    // Terminates
    auto result = Explore(params, [] {
      twist::ed::std::atomic<bool> f{false};

      twist::ed::std::thread t([&f] {
        f.store(true);
      });

      while (!f.load()) {
        twist::assist::Prune("Ignore spin loops");
      }

      t.join();
    });

    REQUIRE(!result.crash);
  }

  SECTION("max_preemptions") {
    auto params = twist::sim::sched::DfsScheduler::Params{};
    params.max_preemptions = 1;

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      auto incrs = [&atom] {
        for (size_t i = 0; i < 100; ++i) {
          atom.fetch_add(1);
        }
      };

      twist::ed::std::thread t1(incrs);
      twist::ed::std::thread t2(incrs);

      t1.join();
      t2.join();

      TWIST_ASSERT(atom.load() == 200, "Missing increments");
    });

    REQUIRE(!result.crash);
    fmt::println("Incr simulations = {}", result.count);
  }

  SECTION("Random") {
    // test::Either, assist::Choice

    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      twist::ed::std::atomic<size_t> atom{0};

      if (twist::test::Either()) {
        atom.fetch_add(1);
      } else {
        atom.fetch_add(2);
      }

      twist::ed::std::random_device rd;
      twist::assist::Choice choice{rd};

      auto r1 = choice(2);
      auto r2 = choice(3);
      auto r3 = choice(4);

      WHEELS_UNUSED(r1 + r2 + r3);
    });

    REQUIRE(!result.crash);
    REQUIRE(result.count == 2 * 2 * 3 * 4);
  }

  SECTION("SpinLock") {
    // SpinLock

    class SpinLock {
     public:
      void Lock() {
        while (!TryLock()) {
          // Try again
        }
      }

      bool TryLock() {
        return !locked_.exchange(true);
      }

      void Unlock() {
        locked_.store(false);
      }

     private:
      twist::ed::std::atomic<bool> locked_{false};
    };

    auto params = twist::sim::sched::DfsScheduler::Params{};
    params.max_steps = 15;
    params.force_preempt_after_steps = 5;
    params.max_preemptions = 3;

    auto result = Explore(params, [] {
      SpinLock spinlock;

      auto contender = [&] {
        while (true) {
          if (twist::test::Either()) {
            spinlock.Lock();
            spinlock.Unlock();
          } else {
            if (spinlock.TryLock()) {
              spinlock.Unlock();
            }
          }
        }
      };

      twist::ed::std::thread t1(contender);
      twist::ed::std::thread t2(contender);

      t1.join();
      t2.join();
    });

    fmt::println("SpinLock simulations: {}", result.count);
  }

  SECTION("Interleaving #1") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      twist::test::WaitGroup{}
          .Add(1, [&] {
            atom.fetch_add(1);
            atom.fetch_add(1);
            atom.fetch_add(1);
          })
          .Join();
    });

    REQUIRE(result.count == 1);

    fmt::println("Interleaving 1: {}", result.count);
  }

  SECTION("Interleaving #2") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      twist::test::WaitGroup{}
          .Add(2, [&] {
            atom.fetch_add(1);
            atom.fetch_add(1);
            atom.fetch_add(1);
            atom.fetch_add(1);
          })
          .Join();
    });

    // https://math.stackexchange.com/questions/77721/number-of-instruction-interleaving
    REQUIRE(result.count == 252);

    fmt::println("Interleaving 2: {}", result.count);
  }

  SECTION("Interleaving #3") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      twist::test::WaitGroup{}
          .Add(3, [&] {
            atom.fetch_add(1);
            atom.fetch_add(1);
          })
          .Join();
    });

    // https://math.stackexchange.com/questions/77721/number-of-instruction-interleaving
    REQUIRE(result.count == 1680);

    fmt::println("Interleaving 3: {}", result.count);
  }

  SECTION("Interleaving #4") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      twist::test::WaitGroup{}
          .Add(2, [&] {
            twist::assist::PreemptionPoint();
          })
          .Join();
    });

    // 1 1 2 2
    // 1 2 1 2
    // 1 2 2 1
    // + symmetric
    REQUIRE(result.count == 6);

    fmt::println("Interleaving 4: {}", result.count);
  }

  SECTION("Interleaving #5") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      struct Widget {};

      twist::test::WaitGroup{}
          .Add(2, [&] {
            Widget* w = twist::assist::New<Widget>();
            delete w;
          })
          .Join();
    });

    // 1 1 2 2
    // 1 2 1 2
    // 1 2 2 1
    // + symmetric
    REQUIRE(result.count == 6);

    fmt::println("Interleaving 5: {}", result.count);
  }

  SECTION("Ptr") {
    auto params = twist::sim::sched::DfsScheduler::Params{};

    auto result = Explore(params, [] {
      struct Widget {
        void Foo() {}
      };

      Widget* w = new Widget{};

      twist::test::WaitGroup{}
          .Add([w] {
            delete w;
          })
          .Add([w] {
            twist::assist::Ptr(w)->Foo();
          })
          .Join();
    });

    REQUIRE(result.crash);

    fmt::println("Widget: {}", (*result.crash).status);
  }

  SECTION("Spurious compare_exchange_weak failure") {
    auto params = twist::sim::sched::DfsScheduler::Params{};
    params.spurious_failures = true;

    auto result = Explore(params, [] {
      twist::ed::std::atomic<int> atom{0};

      int e = 0;
      if (!atom.compare_exchange_weak(e, 1)) {
        twist::ed::c::abort();
      }
    });

    REQUIRE(result.crash);

    fmt::println("Abort: {}", (*result.crash).status);
  }

  {
    // Spin loop pruning

//    auto params = twist::sim::sched::DfsScheduler::Params{};
//
//    auto result = Explore(params, [] {
//      twist::ed::std::atomic<bool> flag{false};
//
//      twist::ed::std::thread t([&] {
//        flag.store(true);
//      });
//
//      twist::ed::SpinWait spin_wait;
//      while (!flag.load()) {
//        spin_wait();
//      }
//
//      t.join();
//    });
//
//    assert(!result.crash);
  }
}
