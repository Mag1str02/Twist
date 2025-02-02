#include <wheels/test/util/cpu_timer.hpp>

#include <wheels/test/framework.hpp>

#include <atomic>
#include <chrono>
#include <thread>
#include <random>

using namespace std::chrono_literals;

using wheels::ProcessCPUTimer;
using wheels::ThreadCPUTimer;

class Work {
 public:
  void Do() {
    value_ ^= twister_();
  }
 private:
  size_t value_ = 0;
  std::mt19937 twister_;
};

TEST_SUITE(CPUTime) {
  double ToSeconds(std::chrono::nanoseconds elapsed) {
    return (double)elapsed.count() / 1'000'000'000;
  }

  SIMPLE_TEST(Work) {
    ThreadCPUTimer timer;
    Work work;
    for (size_t i = 0; i < 100000000; ++i) {
      work.Do();
    }
    ASSERT_TRUE(timer.Elapsed() > 100ms);
  }

  SIMPLE_TEST(Sleep) {
    ThreadCPUTimer timer;
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(timer.Elapsed() < 10ms);
  }

  SIMPLE_TEST(PerThread) {
    std::atomic<bool> stop_requested{false};

    std::thread busy([&]() {
      ThreadCPUTimer timer;
      Work work;
      while (!stop_requested.load()) {
        work.Do();
      }
      auto elapsed = timer.Elapsed();
      std::cout << "Busy thread running time: " << ToSeconds(elapsed) << std::endl;
      ASSERT_TRUE(elapsed > 700ms);
    });

    ThreadCPUTimer timer;

    std::this_thread::sleep_for(1s);

    auto elapsed = timer.Elapsed();
    std::cout << "Sleeper running time: " << ToSeconds(elapsed) << std::endl;
    ASSERT_TRUE(elapsed < 100ms);

    stop_requested.store(true);
    busy.join();
  }
}
