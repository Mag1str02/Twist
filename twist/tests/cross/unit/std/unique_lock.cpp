#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/shared_mutex.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/atomic.hpp>

#include <chrono>


using namespace std::chrono_literals;

template <typename MutexType>
void CheckLocked(MutexType& mu) {
  twist::ed::std::thread t([&mu] {
    for (size_t i = 0; i < 5; ++i) {
      ASSERT_FALSE(mu.try_lock());
    }
  });
  t.join();
}

TEST_SUITE(UniqueLock) {

  template <typename MutexType>
  void TestUniqueLock() {
    twist::cross::Run([] {
      {
        // Just works
        MutexType mutex;

        {
          twist::ed::std::unique_lock lock{mutex};
          CheckLocked(mutex);
        }

        // NB: Spurious failures
        while (!mutex.try_lock()) {
        }

        mutex.unlock();
      }

      {
        // Unlock
        MutexType mutex;

        {
          twist::ed::std::unique_lock lock{mutex};
          lock.unlock();
        }
      }

      {
        // Manual locking
        MutexType mutex;

        twist::ed::std::unique_lock lock{mutex};
        lock.unlock();

        ASSERT_FALSE(lock.owns_lock());

        lock.lock();

        CheckLocked(mutex);

        ASSERT_TRUE(lock.owns_lock());

        lock.unlock();

        {
          mutex.lock();
          mutex.unlock();
        }

        // NB: Spurious failures
        while (!lock.try_lock()) {
        }

        CheckLocked(mutex);

        lock.unlock();
      }

      {
        // Observers

        MutexType mutex;

        {
          twist::ed::std::unique_lock lock{mutex};
          ASSERT_TRUE(lock.owns_lock());
          ASSERT_EQ(lock.mutex(), &mutex);
          ASSERT_TRUE(lock);
        }
      }

      {
        // Ctor

        MutexType mutex;

        {
          // Defer lock

          twist::ed::std::unique_lock lock{mutex, std::defer_lock};
          ASSERT_EQ(lock.mutex(), &mutex);
          ASSERT_FALSE(lock.owns_lock());
          ASSERT_FALSE(lock);

          lock.lock();
          ASSERT_TRUE(lock.owns_lock());
        }

        {
          // Adopt lock

          mutex.lock();

          twist::ed::std::unique_lock lock{mutex, std::adopt_lock};
          ASSERT_EQ(lock.mutex(), &mutex);
          ASSERT_TRUE(lock.owns_lock());
          ASSERT_TRUE(lock);

          lock.unlock();
        }

        {
          // Try lock (Success)

          while (true) {
            twist::ed::std::unique_lock lock{mutex, std::try_to_lock};
            if (lock.owns_lock()) {
              break;
            }
          }
        }

        {
          // Try lock (Failure)

          mutex.lock();

          twist::ed::std::unique_lock lock{mutex, std::try_to_lock};
          ASSERT_FALSE(lock.owns_lock());
        }
      }

      {
        // Move ctor

        MutexType mutex;

        {
          twist::ed::std::unique_lock lock1{mutex};
          ASSERT_TRUE(lock1.owns_lock());
          ASSERT_EQ(lock1.mutex(), &mutex);

          auto lock2 = std::move(lock1);

          ASSERT_TRUE(lock2.owns_lock());
          ASSERT_EQ(lock2.mutex(), &mutex);

          ASSERT_FALSE(lock1.owns_lock());
          ASSERT_EQ(lock1.mutex(), nullptr);
        }
      }

      {
        // Move assignment

        MutexType mutex1;
        MutexType mutex2;

        {
          twist::ed::std::unique_lock lock1{mutex1};
          ASSERT_TRUE(lock1.owns_lock());
          ASSERT_EQ(lock1.mutex(), &mutex1);

          twist::ed::std::unique_lock lock2{mutex2};
          ASSERT_TRUE(lock2.owns_lock());
          ASSERT_EQ(lock2.mutex(), &mutex2);

          lock2 = std::move(lock1);

          ASSERT_TRUE(lock2.owns_lock());
          ASSERT_EQ(lock2.mutex(), &mutex1);

          ASSERT_FALSE(lock1.owns_lock());
          ASSERT_EQ(lock1.mutex(), nullptr);
        }
      }
    });
  }

  SIMPLE_TEST(StdMutex) {
    TestUniqueLock<twist::ed::std::mutex>();
  }

  SIMPLE_TEST(SharedMutex) {
    TestUniqueLock<twist::ed::std::shared_mutex>();
  }

  class SpinLock {
   public:
    void lock() {
      while (locked_.exchange(true)) {
        //
      }
    }

    bool try_lock() {
      return locked_.exchange(true) == false;
    }

    void unlock() {
      locked_.store(false);
    }

   private:
    twist::ed::std::atomic_bool locked_{false};
  };

  SIMPLE_TEST(SpinLock) {
    TestUniqueLock<SpinLock>();
  }
}
