#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>

using twist::ed::std::atomic;
using twist::ed::std::atomic_flag;
using twist::ed::std::thread;

namespace this_thread = twist::ed::std::this_thread;

using namespace std::chrono_literals;

#if defined(__TWIST_ATOMIC_WAIT__)

TEST_SUITE(StdAtomicWait) {
  SIMPLE_TEST(NotifyOne) {
    twist::cross::Run([] {
      atomic<int64_t> f{0};

      thread t([&] {
        this_thread::sleep_for(1s);
        f.store(1);
        f.notify_one();
      });

      f.wait(0);
      ASSERT_EQ(f.load(), 1);

      t.join();
    });
  }

  SIMPLE_TEST(DoNotWait) {
    twist::cross::Run([] {
      atomic<int64_t> f{1};

      f.wait(0);
    });
  }

  SIMPLE_TEST(NotifyAll) {
    twist::cross::Run([] {
      atomic<int64_t> f{0};

      atomic<size_t> wake{0};

      auto waiter = [&] {
        f.wait(0);
        ASSERT_EQ(f.load(), 1);
        ++wake;
      };

      thread t1(waiter);
      thread t2(waiter);

      this_thread::sleep_for(1s);

      ASSERT_EQ(wake.load(), 0);

      f.store(1);
      f.notify_all();

      t1.join();
      t2.join();

      ASSERT_EQ(wake.load(), 2);
    });
  }

  struct Test {
    int x;
  };

  SIMPLE_TEST(WaitStruct) {
    twist::cross::Run([] {
      atomic<Test> a{Test{0}};

      thread t([&] {
        this_thread::sleep_for(1s);
        a.store(Test{1});
        a.notify_one();
      });

      a.wait(Test{0});
      {
        Test v = a.load();
        ASSERT_EQ(v.x, 1);
      }

      t.join();
    });
  }

  SIMPLE_TEST(MemoryOrder) {
    twist::cross::Run([] {
      atomic<bool> f{false};

      // Compiles
      f.wait(true, std::memory_order::acquire);
    });
  }
}

TEST_SUITE(StdAtomicFlagWait) {
  SIMPLE_TEST(NotifyOne) {
    twist::cross::Run([] {
      atomic_flag f;

      ASSERT_FALSE(f.test());

      thread t([&] {
        this_thread::sleep_for(1s);
        f.test_and_set();
        f.notify_one();
      });

      f.wait(false);
      ASSERT_TRUE(f.test());

      t.join();
    });
  }

  SIMPLE_TEST(DoNotWait) {
    twist::cross::Run([] {
      atomic_flag f;

      f.wait(true);
    });
  }

  SIMPLE_TEST(NotifyAll) {
    twist::cross::Run([] {
      atomic_flag f;

      atomic<size_t> wake{0};

      auto waiter = [&] {
        f.wait(false);
        ASSERT_TRUE(f.test());
        ++wake;
      };

      thread t1(waiter);
      thread t2(waiter);

      this_thread::sleep_for(1s);

      ASSERT_EQ(wake.load(), 0);

      f.test_and_set();
      f.notify_all();

      t1.join();
      t2.join();

      ASSERT_EQ(wake.load(), 2);
    });
  }

  SIMPLE_TEST(MemoryOrder) {
    twist::cross::Run([] {
      atomic_flag f;

      // Compiles
      f.wait(true, std::memory_order::acquire);
    });
  }
}

#endif
