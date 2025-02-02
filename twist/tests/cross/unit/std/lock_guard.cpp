#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/mutex.hpp>

using namespace std::chrono_literals;

TEST_SUITE(LockGuard) {
  SIMPLE_TEST(JustWorks) {
    twist::cross::Run([] {
      twist::ed::std::mutex mutex;

      twist::ed::std::lock_guard g{mutex};
    });
  }

  struct TestMutex {
    void lock() {
      ASSERT_FALSE(locked);
      locked = true;
    }

    void unlock() {
      ASSERT_TRUE(locked);
      locked = false;
    }

    bool locked = false;
  };

  SIMPLE_TEST(UserMutex) {
    twist::cross::Run([] {
      TestMutex mutex;

      ASSERT_FALSE(mutex.locked);

      {
        twist::ed::std::lock_guard g{mutex};

        ASSERT_TRUE(mutex.locked);
      }

      ASSERT_FALSE(mutex.locked);
    });
  }
}
