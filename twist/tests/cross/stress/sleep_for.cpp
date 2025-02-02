#include <twist/cross.hpp>

#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/random.hpp>

#include <wheels/core/stop_watch.hpp>

#include <chrono>
#include <vector>

using namespace std::chrono_literals;

void TestSleepFor() {
  wheels::StopWatch<> sw;
  while (sw.Elapsed() < 3s) {
    twist::cross::Run([] {
      twist::ed::std::random_device rd{};
      std::mt19937 mt{rd()};

      size_t count = 1 + mt() % 10;

      std::vector<twist::ed::std::thread> sleepers;
      sleepers.reserve(count);

      std::uniform_int_distribution<uint32_t> delays(1, 1'000'000);
      for (size_t i = 0; i < count; ++i) {
        auto delay = 1us * delays(mt);
        sleepers.emplace_back([delay] {
          twist::ed::std::this_thread::sleep_for(delay);
        });
      }

      for (auto& t : sleepers) {
        t.join();
      }
    });
  }
}

int main() {
  TestSleepFor();
  return 0;
}
