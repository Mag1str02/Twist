#pragma once

#include <wheels/test/test_options.hpp>

#include <atomic>
#include <thread>
#include <chrono>

namespace wheels::test {

class TestTimeLimitWatcher {
  using Clock = std::chrono::steady_clock;

 public:
  TestTimeLimitWatcher(std::chrono::milliseconds time_limit);

  ~TestTimeLimitWatcher() {
    Join();
  }

 private:
  void Watch();
  void Join();

 private:
  std::chrono::milliseconds time_limit_;
  std::atomic<bool> stop_requested_{false};
  std::thread watcher_thread_;
};

std::chrono::milliseconds TestTimeLimit(TestOptions options);

}  // namespace wheels::test
