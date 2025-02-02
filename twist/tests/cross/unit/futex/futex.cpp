#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/chrono.hpp>
#include <twist/ed/wait/futex.hpp>

#include <fmt/core.h>

using twist::ed::std::atomic;
using twist::ed::std::thread;

namespace this_thread = twist::ed::std::this_thread;

namespace futex = twist::ed::futex;

using namespace std::chrono_literals;

TEST_SUITE(Futex) {
  SIMPLE_TEST(Wait) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{0};

      thread producer([&] {
        this_thread::sleep_for(1s);

        {
          auto wake_key = futex::PrepareWake(flag);
          flag.store(1);
          futex::WakeOne(wake_key);
        }
      });

      uint32_t f = futex::Wait(flag, 0);
      ASSERT_EQ(f, 1);
      ASSERT_EQ(flag.load(), 1);

      producer.join();
    });
  }

  SIMPLE_TEST(WakeKey) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{0};
      auto key1 = futex::PrepareWake(flag);

      auto key2 = key1;  // Copyable
      WHEELS_UNUSED(key2);
    });
  }

  SIMPLE_TEST(DoNotWait) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{1};

      futex::Wait(flag, 0);
      ASSERT_EQ(flag.load(), 1);
    });
  }

  SIMPLE_TEST(WaitTimed1) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{0};

      thread producer([&] {
        this_thread::sleep_for(1s);

        {
          auto wake_key = futex::PrepareWake(flag);
          flag.store(1);
          futex::WakeOne(wake_key);
        }
      });

      auto [wake, f] = futex::WaitTimed(flag, 0, 100ms);
      ASSERT_FALSE(wake);  // Timeout
      ASSERT_EQ(flag.load(), 0);

      producer.join();
    });
  }

  SIMPLE_TEST(WaitTimed2) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{0};

      thread producer([&] {
        this_thread::sleep_for(1s);

        {
          auto wake_key = futex::PrepareWake(flag);
          flag.store(1);
          futex::WakeOne(wake_key);
        }
      });

      auto [wake, f] = futex::WaitTimed(flag, 0, 10s);
      ASSERT_TRUE(wake);
      ASSERT_EQ(f, 1);
      ASSERT_EQ(flag.load(), 1);

      producer.join();
    });
  }

  SIMPLE_TEST(DoNotWaitTimed) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{1};

      auto [wake, f] = futex::WaitTimed(flag, 0, 1s);
      ASSERT_TRUE(wake);
      ASSERT_EQ(f, 1);
      ASSERT_EQ(flag.load(), 1);
    });
  }

  SIMPLE_TEST(MemoryOrder) {
    twist::cross::Run([] {
      atomic<uint32_t> flag{1};

      {
        // Compiles
        futex::Wait(flag, 0, std::memory_order::acquire);
      }

      {
        // Compiles
        futex::WaitTimed(flag, 0, 1s, std::memory_order::acquire);
      }
    });
  }

  uint64_t GetLow(uint64_t v) {
    return v & 0xFFFFFFFF;
  }

  uint64_t GetHigh(uint64_t v) {
    return (v >> 32) & 0xFFFFFFFF;
  }

  SIMPLE_TEST(LowHigh) {
    twist::cross::Run([] {
      const uint64_t kLowPing = 1;
      const uint64_t kHighPong = (uint64_t)1 << 32;

      atomic<uint64_t> state{kHighPong};

      auto low_ref = futex::RefLowHalf(state);
      auto high_ref = futex::RefHighHalf(state);

      const size_t kRounds = 3;

      thread t([&, high_ref, low_ref] {
        auto high_wake_key = futex::PrepareWake(high_ref);

        for (size_t j = 0; j < kRounds; ++j) {
          uint64_t ws = futex::Wait(low_ref, 0);  // await {?, 1}

          ASSERT_TRUE(GetLow(ws) == 1);

          uint64_t s = state.load();

          ASSERT_TRUE(GetLow(s) == 1);

          this_thread::sleep_for(1s);

          state.exchange(kHighPong);

          futex::WakeOne(high_wake_key);
        }
      });

      auto low_wake_key = futex::PrepareWake(low_ref);

      for (size_t i = 0; i < kRounds; ++i) {
        uint64_t ws = futex::Wait(high_ref, 0);  // await {1, ?}

        ASSERT_TRUE(GetHigh(ws) == 1);

        uint64_t s = state.load();

        ASSERT_TRUE(GetHigh(s) == 1);

        this_thread::sleep_for(1s);

        state.exchange(kLowPing);

        futex::WakeOne(low_wake_key);
      }

      t.join();
    });
  }
}
