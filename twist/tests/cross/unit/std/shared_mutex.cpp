#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/shared_mutex.hpp>
#include <twist/ed/std/thread.hpp>

#include <chrono>

using twist::ed::std::shared_mutex;
using twist::ed::std::thread;

namespace this_thread = twist::ed::std::this_thread;

using namespace std::chrono_literals;

TEST_SUITE(StdSharedMutex) {
  SIMPLE_TEST(Writer) {
    twist::cross::Run([] {
      shared_mutex m;

      m.lock();
      m.unlock();
    });
  }

  SIMPLE_TEST(WriterTryLock) {
    twist::cross::Run([] {
      shared_mutex m;

      {
        // Allows spurious failures
        while (!m.try_lock()) {
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
    });
  }

  SIMPLE_TEST(OneReader) {
    twist::cross::Run([] {
      shared_mutex m;

      m.lock_shared();
      m.unlock_shared();
    });
  }

  SIMPLE_TEST(TwoReaders) {
    twist::cross::Run([] {
      shared_mutex m;

      m.lock_shared();
      m.lock_shared();

      m.unlock_shared();
      m.unlock_shared();
    });
  }

  SIMPLE_TEST(ReaderTryLock) {
    twist::cross::Run([] {
      shared_mutex m;

      m.lock_shared();

      {
        // Allows spurious failures
        while (!m.try_lock_shared()) {
        }
      }

      m.unlock_shared();
      m.unlock_shared();
    });
  }

  SIMPLE_TEST(Multi) {
    twist::cross::Run([] {
      shared_mutex m;

      for (size_t i = 0; i < 128; ++i) {
        {
          m.lock();
          m.unlock();
        }

        {
          m.lock_shared();
          m.unlock_shared();
        }
      }
    });
  }

  SIMPLE_TEST(WriterXorReaders) {
    twist::cross::Run([] {
      shared_mutex m;

      {
        m.lock();

        {
          for (size_t i = 0; i < 100; ++i) {
            // Writer
            ASSERT_FALSE(m.try_lock());
          }
          for (size_t j = 0; j < 100; ++j) {
            // Reader
            ASSERT_FALSE(m.try_lock_shared());
          }
        }

        m.unlock();
      }

      {
        m.lock_shared();

        {
          // Writer
          for (size_t i = 0; i < 100; ++i) {
            ASSERT_FALSE(m.try_lock());
          }
        }

        {
          // Reader
          // Allows spurious failures
          while (!m.try_lock_shared()) {
          }
          m.unlock_shared();
        }

        m.unlock_shared();
      }
    });
  }

  SIMPLE_TEST(Waiter) {
    twist::cross::Run([] {
      shared_mutex m;

      {
        // Writer

        m.lock();

        thread t([&] {
          m.lock();
          m.unlock();
        });

        this_thread::sleep_for(1s);
        m.unlock();

        t.join();
      }

      {
        // Reader

        m.lock();

        thread t([&] {
          m.lock_shared();
          m.unlock_shared();
        });

        this_thread::sleep_for(1s);
        m.unlock();

        t.join();
      }
    });
  }
}
