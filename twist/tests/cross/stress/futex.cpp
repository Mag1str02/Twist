#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/random.hpp>
#include <twist/ed/wait/futex.hpp>

#include <wheels/core/stop_watch.hpp>

#include <chrono>

using namespace std::chrono_literals;

void TestFutexWait() {
  wheels::StopWatch<> sw;
  while (sw.Elapsed() < 3s) {
    twist::cross::Run([] {
      twist::ed::std::atomic<uint32_t> flag{0};

      twist::ed::std::thread t([&] {
        auto wake_key = twist::ed::futex::PrepareWake(flag);
        flag.store(1);
        twist::ed::futex::WakeOne(wake_key);
      });

      twist::ed::futex::Wait(flag, /*old=*/0);

      t.join();
    });
  }
}

void TestFutexWaitTimed() {
  wheels::StopWatch<> sw;
  while (sw.Elapsed() < 3s) {
    twist::cross::Run([] {
      twist::ed::std::atomic<uint32_t> flag{0};

      twist::ed::std::random_device rd;
      std::mt19937 mt{rd()};
      std::uniform_int_distribution<uint32_t> delays(500, 1500);

      twist::ed::std::thread t([&] {
        auto wake_key = twist::ed::futex::PrepareWake(flag);

        auto delay = 1ms * delays(mt);
        twist::ed::std::this_thread::sleep_for(delay);

        flag.store(1);
        twist::ed::futex::WakeOne(wake_key);
      });

      [[maybe_unused]] auto [woken, new_value] = twist::ed::futex::WaitTimed(flag, /*old=*/0, 1s);

      t.join();
    });
  }
}

int main() {
  TestFutexWait();
  TestFutexWaitTimed();
  return 0;
}
