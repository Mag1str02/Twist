#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/thread.hpp>

#include <catch2/catch_test_macros.hpp>

static_assert(twist::build::Sim());

TEST_CASE("Spurious") {
  SECTION("compare_exchange_weak") {
    // std::atomic<T>::compare_exchange_weak
    // https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange

    twist::sim::sched::RandomScheduler random{{.seed=42}};
    twist::sim::Simulator sim{&random};

    sim.Run([] {
      twist::ed::std::atomic<int> a{0};

      while (true) {
        int v = a.load();
        if (!a.compare_exchange_weak(v, v + 1)) {
          break;  // Spurious failure
        }
      }
    });
  }

  SECTION("compare_exchange_strong") {
    twist::sim::sched::RandomScheduler random{{.seed=42}};
    twist::sim::Simulator sim{&random};

    sim.Run([] {
      twist::ed::std::atomic<int> a{0};

      for (size_t i = 0; i < 1024; ++i) {
        int v = a.load();
        bool success = a.compare_exchange_strong(v, v + 1);
        assert(success);
      }
    });
  }

  SECTION("std::condition_variable::wait") {
    // std::condition_variable::wait
    // https://en.cppreference.com/w/cpp/thread/condition_variable/wait

    bool spurious = false;

    twist::sim::sched::RandomScheduler random{{.seed=42}};

    do {
      twist::sim::Simulator sim{&random};

      sim.Run([&spurious] {
        bool ready;
        twist::ed::std::mutex mu;
        twist::ed::std::condition_variable cv;

        ready = false;

        twist::ed::std::thread t([&] {
          // Producer
          std::lock_guard guard{mu};
          ready = true;
          cv.notify_one();
        });

        {
          // Consumer
          std::unique_lock lock{mu};

          if (!ready) {
            cv.wait(lock);
          }

          if (!ready) {
            spurious = true;  // Spurious wake-up
            cv.wait(lock, [&] {
              return ready;
            });
          }
        }

        t.join();
      });
    } while (!spurious && random.NextSchedule());
  }

  SECTION("std::mutex::try_lock") {
    // std::mutex::try_lock
    // https://en.cppreference.com/w/cpp/thread/mutex/try_lock

    twist::sim::sched::RandomScheduler random{{.seed=42}};
    twist::sim::Simulator sim{&random};

    sim.Run([] {
      twist::ed::std::mutex mu;

      while (true) {
        if (mu.try_lock()) {
          mu.unlock();
        } else {
          break;  // Spurious failure
        }
      }
    });
  }
}
