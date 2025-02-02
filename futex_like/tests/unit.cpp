#include <futex_like/wait_wake.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cassert>
#include <atomic>
#include <thread>

using namespace std::chrono_literals;

TEST_CASE("Wait on flag", "[Wait][WakeOne]") {
  std::atomic_uint32_t flag{0};

  std::thread producer([&] {
    std::this_thread::sleep_for(1s);

    {
      auto wake_key = futex_like::PrepareWake(flag);
      flag.store(1);
      futex_like::WakeOne(wake_key);
    }
  });

  uint32_t f = futex_like::Wait(flag, 0);
  assert(f == 1);
  assert(flag.load() == 1);

  producer.join();
}

TEST_CASE("WakeOne", "[Wait][WakeOne]") {
  std::atomic_size_t counter = 0;

  std::atomic_uint32_t flag = 0;

  std::thread t1([&] {
    futex_like::Wait(flag, 0);
    ++counter;
  });

  std::thread t2([&] {
    futex_like::Wait(flag, 0);
    ++counter;
  });

  std::this_thread::sleep_for(1s);

  assert(counter.load() == 0);

  auto waiters = futex_like::PrepareWake(flag);
  flag.store(1);
  futex_like::WakeOne(waiters);

  std::this_thread::sleep_for(1s);

  assert(counter.load() == 1);

  futex_like::WakeAll(waiters);

  t1.join();
  t2.join();
}

TEST_CASE("WakeAll", "[Wait][WakeAll]") {
  std::atomic_uint32_t flag = 0;

  std::thread t1([&] {
    futex_like::Wait(flag, 0);
  });

  std::thread t2([&] {
    futex_like::Wait(flag, 0);
  });

  std::this_thread::sleep_for(1s);

  auto waiters = futex_like::PrepareWake(flag);
  flag.store(1);
  futex_like::WakeAll(waiters);

  t1.join();
  t2.join();
}

TEST_CASE("WakeKey", "[WakeKey]") {
  std::atomic_uint32_t flag{0};
  auto key1 = futex_like::PrepareWake(flag);

  auto key2 = key1;  // Copyable
  (void)key2;
}

TEST_CASE("DoNotWait", "[Wait]") {
  std::atomic_uint32_t flag{1};

  futex_like::Wait(flag, 0);
  assert(flag.load() == 1);
}

TEST_CASE("WaitTimed", "[WaitTimed][WakeOne]") {
  std::atomic_uint32_t flag{0};

  std::thread producer([&] {
    std::this_thread::sleep_for(1s);

    {
      auto wake_key = futex_like::PrepareWake(flag);
      flag.store(1);
      futex_like::WakeOne(wake_key);
    }
  });

  auto [wake, f] = futex_like::WaitTimed(flag, 0, 100ms);
  assert(!wake);  // Timeout
  assert(flag.load() == 0);

  producer.join();
}

TEST_CASE("WaitTimed2", "[WaitTimed][WakeOne]") {
  std::atomic_uint32_t flag{0};

  std::thread producer([&] {
    std::this_thread::sleep_for(1s);

    {
      auto wake_key = futex_like::PrepareWake(flag);
      flag.store(1);
      futex_like::WakeOne(wake_key);
    }
  });

  auto [wake, f] = futex_like::WaitTimed(flag, 0, 10s);
  assert(wake);
  assert(f == 1);
  assert(flag.load() == 1);

  producer.join();
}

TEST_CASE("DoNotWaitTimed", "[WaitTimed]") {
  std::atomic_uint32_t flag{1};

  auto [wake, f] = futex_like::WaitTimed(flag, 0, 1s);
  assert(wake);
  assert(f == 1);
  assert(flag.load() == 1);
}

TEST_CASE("MemoryOrder", "[MemoryOrder][Wait][WaitTimed]") {
  std::atomic_uint32_t flag{1};

  {
    // Compiles
    futex_like::Wait(flag, 0, std::memory_order::acquire);
  }

  {
    // Compiles
    futex_like::WaitTimed(flag, 0, 1s, std::memory_order::acquire);
  }
}

uint64_t GetLow(uint64_t v) {
  return v & 0xFFFFFFFF;
}

uint64_t GetHigh(uint64_t v) {
  return (v >> 32) & 0xFFFFFFFF;
}

TEST_CASE("AtomicUint64", "[RefLowHalf][RefHighHalf][Wait][WakeOne]") {
  const uint64_t kLowPing = 1;
  const uint64_t kHighPong = (uint64_t)1 << 32;

  std::atomic<uint64_t> state{kHighPong};

  auto low_ref = futex_like::RefLowHalf(state);
  auto high_ref = futex_like::RefHighHalf(state);

  const size_t kRounds = 3;

  std::thread t([&, high_ref, low_ref] {
    auto high_wake_key = futex_like::PrepareWake(high_ref);

    for (size_t j = 0; j < kRounds; ++j) {
      uint64_t ws = futex_like::Wait(low_ref, 0);  // await {?, 1}

      assert(GetLow(ws) == 1);

      uint64_t s = state.load();

      assert(GetLow(s) == 1);

      std::this_thread::sleep_for(1s);

      state.exchange(kHighPong);

      futex_like::WakeOne(high_wake_key);
    }
  });

  auto low_wake_key = futex_like::PrepareWake(low_ref);

  for (size_t i = 0; i < kRounds; ++i) {
    uint64_t ws = futex_like::Wait(high_ref, 0);  // await {1, ?}

    assert(GetHigh(ws) == 1);

    uint64_t s = state.load();

    assert(GetHigh(s) == 1);

    std::this_thread::sleep_for(1s);

    state.exchange(kLowPing);

    futex_like::WakeOne(low_wake_key);
  }

  t.join();
}
