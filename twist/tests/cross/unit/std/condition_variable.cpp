#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/chrono.hpp>

#include <chrono>
#include <vector>

using twist::ed::std::mutex;
using twist::ed::std::condition_variable;
using twist::ed::std::thread;

using twist::ed::std::chrono::system_clock;
using twist::ed::std::chrono::steady_clock;

namespace this_thread = twist::ed::std::this_thread;

using namespace std::chrono_literals;

TEST_SUITE(StdConditionVariable) {
  class Event {
   public:
    void Set() {
      {
        std::lock_guard guard{mutex_};
        flag_ = true;
      }
      cv_.notify_one();
    }

    void Wait() {
      std::unique_lock lock{mutex_};
      while (!flag_) {
        cv_.wait(lock);
      }
    }

   private:
    mutex mutex_;
    condition_variable cv_;
    bool flag_ = false;
  };

  SIMPLE_TEST(NotifyOne) {
    twist::cross::Run([] {
      int data = 0;
      Event e;

      thread producer([&] {
        data = 42;
        e.Set();
      });

      e.Wait();
      ASSERT_EQ(data, 42);

      producer.join();
    });
  }

  class Barrier {
   public:
    explicit Barrier(size_t count)
        : left_(count) {
    }

    void Arrive() {
      std::unique_lock lock{mutex_};
      if (--left_ == 0) {
        // Last
        cv_.notify_all();
      } else {
        while (left_ > 0) {
          cv_.wait(lock);
        }
      }
    }

   private:
    mutex mutex_;
    condition_variable cv_;
    size_t left_;
  };

  SIMPLE_TEST(NotifyAll) {
    twist::cross::Run([] {
      std::vector<thread> threads;

      Barrier barrier{4};

      for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back([i, &barrier] {
          this_thread::sleep_for(100ms * i);
          barrier.Arrive();
        });
      }

      for (auto& t : threads) {
        t.join();
      }
    });
  }

  SIMPLE_TEST(WaitFor) {
    twist::cross::Run([] {
      mutex m;
      condition_variable cv;

      auto start = steady_clock::now();

      {
        std::unique_lock lock{m};
        for (size_t i = 0; i < 10; ++i) {
          std::cv_status status = cv.wait_for(lock, 100ms);
          WHEELS_UNUSED(status);
        }
      }

      auto elapsed = steady_clock::now() - start;
      ASSERT_TRUE(elapsed >= 500ms);
    });
  }

  SIMPLE_TEST(WaitForNoTimeout) {
    twist::cross::Run([] {
      mutex m;
      condition_variable cv;
      bool flag = false;

      thread t([&] {
        // Producer

        this_thread::sleep_for(256ms);

        {
          std::lock_guard guard{m};
          flag = true;
          cv.notify_one();
        }
      });

      {
        // Consumer

        std::unique_lock lock{m};
        while (!flag) {
          std::cv_status status = cv.wait_for(lock, 1s);
          ASSERT_TRUE(status == std::cv_status::no_timeout);
        }
      }

      t.join();
    });
  }

  SIMPLE_TEST(WaitUntil) {
    twist::cross::Run([] {
      mutex m;
      condition_variable cv;

      auto start = steady_clock::now();

      {
        std::unique_lock lock{m};
        for (size_t i = 0; i < 10; ++i) {
          auto later = system_clock::now() + 100ms;
          std::cv_status status = cv.wait_until(lock, later);
          WHEELS_UNUSED(status);
        }
      }

      auto elapsed = steady_clock::now() - start;
      ASSERT_TRUE(elapsed >= 500ms);
    });
  }
}
