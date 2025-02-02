#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>

#include <chrono>

using twist::ed::std::mutex;
using twist::ed::std::thread;

namespace this_thread = twist::ed::std::this_thread;

using namespace std::chrono_literals;

TEST_SUITE(StdMutex) {
  SIMPLE_TEST(JustWorks) {
    twist::cross::Run([] {
      mutex m;

      m.lock();
      m.unlock();
    });
  }

  SIMPLE_TEST(Multi) {
    twist::cross::Run([] {
      mutex m;

      for (size_t i = 0; i < 128; ++i) {
        m.lock();
        m.unlock();
      }
    });
  }

  SIMPLE_TEST(RAII) {
    twist::cross::Run([] {
      mutex m;

      {
        std::lock_guard<mutex> guard{m};
      }

      {
        std::unique_lock<mutex> lock{m};
        ASSERT_TRUE(lock.owns_lock());
        lock.unlock();
      }
    });
  }

  SIMPLE_TEST(TryLock) {
    twist::cross::Run([] {
      mutex m;

      {
        while (!m.try_lock()) {
          // Spurious failure, try again
        }
        m.unlock();
      }

      {
        m.lock();

        for (size_t i = 0; i < 100; ++i) {
          ASSERT_FALSE(m.try_lock());
        }

        m.unlock();
      }

      {
        std::unique_lock lock{m, std::try_to_lock};
        if (lock.owns_lock()) {
          lock.unlock();
        } else {
          // Spurious wake-up
        }
      }
    });
  }

  SIMPLE_TEST(Wait) {
    twist::cross::Run([] {
      mutex m;

      m.lock();

      thread t([&] {
        m.lock();
        m.unlock();
      });

      this_thread::sleep_for(1s);
      m.unlock();

      t.join();
    });
  }
}
