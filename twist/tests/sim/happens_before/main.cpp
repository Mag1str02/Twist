#include <twist/sim.hpp>
#include <twist/mod/sim/switch_to.hpp>

#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/fmt/print.hpp>

#include <twist/assist/shared.hpp>

#include <twist/test/message.hpp>

#include <wheels/core/compiler.hpp>

#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>

#include <optional>

static_assert(twist::build::IsolatedSim());

TEST_CASE("Data race") {
  twist::sim::sched::FairScheduler scheduler{{.seed = 42}};
  twist::sim::Simulator sim{&scheduler};

  auto result = sim.Run([] {
    twist::assist::Shared<int> data{0};

    twist::ed::std::thread t([&] {
      *data = 42;  // W
    });

    int d = data;  // R
    twist::ed::fmt::println("{}", d);

    t.join();
  });

  REQUIRE(!result.Ok());
  REQUIRE(result.status == twist::sim::Status::DataRace);
}

TEST_CASE("Message passing") {
  SECTION("thread::join") {
    twist::sim::sched::FairScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};

      twist::ed::std::thread t([&] {
        *data = 42;  // W
      });

      t.join();

      int d = data;  // R
      twist::ed::fmt::println("{}", d);
    });

    REQUIRE(result.Ok());
  }

  SECTION("store(release) / load(acquire)") {
    twist::sim::sched::FairScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> flag{false};

      twist::ed::std::thread t([&] {
        *data = 42;  // W
        flag.store(true, std::memory_order::release);
      });

      while (!flag.load(std::memory_order::acquire)) {
        twist::ed::std::this_thread::yield();
      };

      int d = data;  // R
      twist::ed::fmt::println("{}", d);

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("store(release) / load(relaxed)") {
    twist::sim::sched::FairScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> flag{false};

      twist::ed::std::thread t([&] {
        *data = 42;  // W
        flag.store(true, std::memory_order::release);
      });

      while (!flag.load(std::memory_order::relaxed)) {
        twist::ed::std::this_thread::yield();
      };

      int d = data;  // R
      twist::ed::fmt::println("{}", d);

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }

  SECTION("store(relaxed) / load(acquire)") {
    twist::sim::sched::FairScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> flag{false};

      twist::ed::std::thread t([&] {
        *data = 42;  // W
        flag.store(true, std::memory_order::relaxed);
      });

      while (!flag.load(std::memory_order::acquire)) {
        twist::ed::std::this_thread::yield();
      };

      int d = data;  // R
      twist::ed::fmt::println("{}", d);

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }

  SECTION("fences") {
    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> flag{false};

      twist::ed::std::thread t2([&] {
        *data = 42;  // W
        twist::ed::std::atomic_thread_fence(std::memory_order::release);
        flag.store(true, std::memory_order::relaxed);
      });

      twist::sim::SwitchTo(2);

      bool f = flag.load(std::memory_order::relaxed);
      assert(f);
      twist::ed::std::atomic_thread_fence(std::memory_order::acquire);

      int d = data;  // R
      twist::ed::fmt::println("{}", d);

      t2.join();
    });

    REQUIRE(result.Ok());
  }
}

TEST_CASE("shared_ptr") {
  SECTION("release sequence") {
    // T1: fa(refcount, rlx)
    // T2: fa(refcount, rlx)
    // T2: W
    // T2: fs(refcount, acq_rel)
    // T3: fa(refcount, rlx)
    // T1: fs(refcount, acq_rel)
    // T3: fs(refcount, acq_rel)
    // T3: R

    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};

      twist::ed::std::atomic<int> refcount{1};  // 1

      twist::ed::std::thread t2([&] {
        int r1 = refcount.fetch_add(1, std::memory_order::relaxed);  // 2
        assert(r1 == 1);

        *data = 42;  // W

        int r2 = refcount.fetch_sub(1, std::memory_order::acq_rel);  // 3
        assert(r2 == 2);

        twist::sim::SwitchTo(3);
      });

      twist::ed::std::thread t3([&] {
        int r1 = refcount.fetch_add(1, std::memory_order::relaxed);  // 4
        assert(r1 == 1);

        twist::sim::SwitchTo(1);

        int r2 = refcount.fetch_sub(1, std::memory_order::acq_rel);  // 6
        assert(r2 == 1);  //  Last decrement

        int d = data;                    // R
        twist::ed::fmt::print("{}", d);  // 42
      });

      twist::sim::SwitchTo(2);

      int r = refcount.fetch_sub(1, std::memory_order::acq_rel);  // 5
      assert(r == 2);

      twist::sim::SwitchTo(3);

      t2.join();
      t3.join();

      assert(refcount.load() == 0);
    });

    REQUIRE(result.Ok());
    REQUIRE(result.std_out == "42");
  }

  SECTION("release sequence, atomic_thread_fence") {
    // T1: fa(refcount, rlx)
    // T2: fa(refcount, rlx)
    // T2: W
    // T2: fs(refcount, rel)
    // T3: fa(refcount, rlx)
    // T1: fs(refcount, rel)
    // T3: fs(refcount, rel)
    // T3: atf(acq)
    // T3: R

    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};

      twist::ed::std::atomic<int> refcount{1};  // 1

      twist::ed::std::thread t2([&] {
        int r1 = refcount.fetch_add(1, std::memory_order::relaxed);  // 2
        assert(r1 == 1);

        *data = 42;  // W

        int r2 = refcount.fetch_sub(1, std::memory_order::release);  // 3
        assert(r2 == 2);

        twist::sim::SwitchTo(3);
      });

      twist::ed::std::thread t3([&] {
        int r1 = refcount.fetch_add(1, std::memory_order::relaxed);  // 4
        assert(r1 == 1);

        twist::sim::SwitchTo(1);

        int r2 = refcount.fetch_sub(1, std::memory_order::release);  // 6
        assert(r2 == 1);  //  Last decrement
        twist::ed::std::atomic_thread_fence(std::memory_order::acquire);  // 7

        int d = data;                    // R
        twist::ed::fmt::print("{}", d);  // 42
      });

      twist::sim::SwitchTo(2);

      int r = refcount.fetch_sub(1, std::memory_order::release);  // 5
      assert(r == 2);

      twist::sim::SwitchTo(3);

      t2.join();
      t3.join();

      assert(refcount.load() == 0);
    });

    REQUIRE(result.Ok());
    REQUIRE(result.std_out == "42");
  }

  SECTION("release sequence, missing atomic_thread_fence") {
    // T1: fa(refcount, rlx)
    // T2: fa(refcount, rlx)
    // T2: W
    // T2: fs(refcount, rel)
    // T3: fa(refcount, rlx)
    // T1: fs(refcount, rel)
    // T3: fs(refcount, rel)
    // T3: missing atf(acq)
    // T3: R

    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};

      twist::ed::std::atomic<int> refcount{1};  // 1

      twist::ed::std::thread t2([&] {
        int r1 = refcount.fetch_add(1, std::memory_order::relaxed);  // 2
        assert(r1 == 1);

        *data = 42;  // W

        int r2 = refcount.fetch_sub(1, std::memory_order::release);  // 3
        assert(r2 == 2);

        twist::sim::SwitchTo(3);
      });

      twist::ed::std::thread t3([&] {
        int r1 = refcount.fetch_add(1, std::memory_order::relaxed);  // 4
        assert(r1 == 1);

        twist::sim::SwitchTo(1);

        int r2 = refcount.fetch_sub(1, std::memory_order::release);  // 6
        assert(r2 == 1);  //  Last decrement

        // twist::ed::std::atomic_thread_fence(std::memory_order::acquire);

        int d = data;                    // R
        twist::ed::fmt::print("{}", d);  // 42
      });

      twist::sim::SwitchTo(2);

      int r = refcount.fetch_sub(1, std::memory_order::release);  // 5
      assert(r == 2);

      twist::sim::SwitchTo(3);

      t2.join();
      t3.join();

      assert(refcount.load() == 0);
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }
}

TEST_CASE("compare_exchange_strong") {
  SECTION("success / memory_order::acquire") {
    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> lock{false};

      twist::ed::std::thread t([&] {
        assert(!lock.exchange(true, std::memory_order::acquire));
        data.Write(42);
        lock.store(true, std::memory_order::release);
      });

      twist::sim::SwitchTo(2);

      bool free = true;
      if (lock.compare_exchange_strong(free, true, std::memory_order::acquire, std::memory_order::relaxed)) {
        int d = data.Read();
        assert(d == 42);
      } else {
        assert(false);
      }

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("success / memory_order::relaxed") {
    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> lock{false};

      twist::ed::std::thread t([&] {
        assert(!lock.exchange(true, std::memory_order::acquire));
        data.Write(42);
        lock.store(true, std::memory_order::release);
      });

      twist::sim::SwitchTo(2);

      bool free = true;
      if (lock.compare_exchange_strong(free, true, std::memory_order::relaxed)) {
        int d = data.Read();  // Data race
        assert(d == 42);
      } else {
        assert(false);
      }

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }

  SECTION("failure / memory_order::acquire") {
    // Consensus example

    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> a;
      twist::assist::Shared<int> b;
      twist::ed::std::atomic<int> consensus{0};

      twist::ed::std::thread t([&] {
        b.Write(2);

        // 0 -> 2
        int init = 0;
        if (consensus.compare_exchange_strong(init, 2, std::memory_order::release, std::memory_order::acquire)) {
          // Win
        } else {
          // Lose
          assert(false);  // Unexpected
        }
      });

      twist::sim::SwitchTo(2);

      a.Write(1);

      // 0 -> 1
      int init = 0;
      if (consensus.compare_exchange_strong(init, 1, std::memory_order::release, std::memory_order::acquire)) {
        // Win
        assert(false);  // Unexpected
      } else {
        // Lose
        int v = b.Read();  // Ok
        assert(v == 2);
      }

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("failure / memory_order::relaxed") {
    // Consensus example

    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> a;
      twist::assist::Shared<int> b;
      twist::ed::std::atomic<int> consensus{0};

      twist::ed::std::thread t([&] {
        b.Write(2);

        // 0 -> 2
        int init = 0;
        if (consensus.compare_exchange_strong(init, 2, std::memory_order::release, std::memory_order::acquire)) {
          // Win
        } else {
          // Lose
          assert(false);  // Unexpected
        }
      });

      twist::sim::SwitchTo(2);

      a.Write(1);

      // 0 -> 1
      int init = 0;
      if (consensus.compare_exchange_strong(init, 1, std::memory_order::release, std::memory_order::relaxed)) {
        // Win
        assert(false);  // Unexpected
      } else {
        // Lose
        int v = b.Read();  // Data race
        assert(v == 2);
      }

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }
}

TEST_CASE("compare_exchange_weak") {
  SECTION("success / memory_order::acquire") {
    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> lock{false};

      twist::ed::std::thread t([&] {
        assert(!lock.exchange(true, std::memory_order::acquire));
        data.Write(42);
        lock.store(true, std::memory_order::release);
      });

      twist::sim::SwitchTo(2);

      bool free = true;
      if (lock.compare_exchange_weak(free, true, std::memory_order::acquire, std::memory_order::relaxed)) {
        int d = data.Read();
        assert(d == 42);
      } else {
        assert(false);
      }

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("success / memory_order::relaxed") {
    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> data{0};
      twist::ed::std::atomic<bool> lock{false};

      twist::ed::std::thread t([&] {
        assert(!lock.exchange(true, std::memory_order::acquire));
        data.Write(42);
        lock.store(true, std::memory_order::release);
      });

      twist::sim::SwitchTo(2);

      bool free = true;
      if (lock.compare_exchange_weak(free, true, std::memory_order::relaxed)) {
        int d = data.Read();  // Data race
        assert(d == 42);
      } else {
        assert(false);
      }

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }

  SECTION("failure / memory_order::acquire") {
    // Consensus example

    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> a;
      twist::assist::Shared<int> b;
      twist::ed::std::atomic<int> consensus{0};

      twist::ed::std::thread t([&] {
        b.Write(2);

        // 0 -> 2
        int init = 0;
        if (consensus.compare_exchange_weak(init, 2, std::memory_order::release, std::memory_order::acquire)) {
          // Win
        } else {
          // Lose
          assert(false);  // Unexpected
        }
      });

      twist::sim::SwitchTo(2);

      a.Write(1);

      // 0 -> 1
      int init = 0;
      if (consensus.compare_exchange_weak(init, 1, std::memory_order::release, std::memory_order::acquire)) {
        // Win
        assert(false);  // Unexpected
      } else {
        // Lose
        int v = b.Read();  // Ok
        assert(v == 2);
      }

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("failure / memory_order::relaxed") {
    // Consensus example

    twist::sim::sched::CoopScheduler scheduler{{.seed=42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      twist::assist::Shared<int> a;
      twist::assist::Shared<int> b;
      twist::ed::std::atomic<int> consensus{0};

      twist::ed::std::thread t([&] {
        b.Write(2);

        // 0 -> 2
        int init = 0;
        if (consensus.compare_exchange_weak(init, 2, std::memory_order::release, std::memory_order::acquire)) {
          // Win
        } else {
          // Lose
          assert(false);  // Unexpected
        }
      });

      twist::sim::SwitchTo(2);

      a.Write(1);

      // 0 -> 1
      int init = 0;
      if (consensus.compare_exchange_weak(init, 1, std::memory_order::release, std::memory_order::relaxed)) {
        // Win
        assert(false);  // Unexpected
      } else {
        // Lose
        int v = b.Read();  // Data race
        assert(v == 2);
      }

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }
}

TEST_CASE("MemoryAllocator") {
  SECTION("Happens-Before") {
    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    struct Node {
      twist::assist::Shared<int> datum{0};
    };

    auto result = sim.Run([] {
      Node* n1 = new Node{};

      twist::assist::Shared<int> racy{0};

      twist::ed::std::thread t([n1, &racy] {
        Node* n2 = new Node{};
        assert(n2 == n1);
        n2->datum.Write(2);
        racy.Write(2);
        WHEELS_UNUSED(racy);

        delete n2;
      });

      n1->datum.Write(1);
      racy.Write(1);

      delete n1;

      twist::sim::SwitchTo(2);

      t.join();
    });

    if (!result.Ok()) {
      fmt::println("{}", result.std_err);
    }

    REQUIRE(result.Ok());
  }
}

TEST_CASE("test::Message") {
  SECTION("Happens-Before") {
    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      using TestMessage = twist::test::Message<int>;

      std::optional<TestMessage> pipe;
      twist::ed::std::atomic_bool flag = false;

      twist::ed::std::thread t([&] {
        // 2
        pipe.emplace(TestMessage::New(7));
        flag.store(true);
      });

      twist::sim::SwitchTo(2);  // 1

      // 3
      if (flag.load()) {  // -> true
        pipe->Read();
      }

      t.join();
    });

    REQUIRE(result.Ok());
  }

  SECTION("Race") {
    twist::sim::sched::CoopScheduler scheduler{{.seed = 42}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      using TestMessage = twist::test::Message<int>;

      std::optional<TestMessage> pipe;
      twist::ed::std::atomic_bool flag = false;

      twist::ed::std::thread t([&] {
        // 2
        pipe.emplace(TestMessage::New(7));
        flag.store(true);
      });

      twist::sim::SwitchTo(2);  // 1

      // 3
      if (flag.load(std::memory_order::relaxed)) {  // -> true
        pipe->Read();  // Data race
      }

      t.join();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::DataRace);
  }
}
