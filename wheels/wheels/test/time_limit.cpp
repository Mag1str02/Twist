#include <wheels/test/time_limit.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/test/helpers.hpp>

#include <wheels/test/support/sanitizers.hpp>

#include <thread>
#include <chrono>

namespace wheels::test {

TestTimeLimitWatcher::TestTimeLimitWatcher(std::chrono::milliseconds time_limit)
    : time_limit_(time_limit), watcher_thread_([this]() { Watch(); }) {
}

void TestTimeLimitWatcher::Watch() {
  auto deadline = Clock::now() + time_limit_;

  do {
    std::this_thread::sleep_for(50ms);
    if (stop_requested_.load()) {
      return;
    }
  } while (Clock::now() < deadline);

  FAIL_TEST("Time limit exceeded - " << ToSeconds(time_limit_) << " seconds");
}

void TestTimeLimitWatcher::Join() {
  stop_requested_.store(true);
  watcher_thread_.join();
}

std::chrono::milliseconds TestTimeLimit(TestOptions options) {
  auto time_limit = options.time_limit;
  if (options.adapt_time_limit_to_sanitizer_) {
    time_limit *= GetSanitizerSlowdown();
  }
  return time_limit;
}

}  // namespace wheels::test
