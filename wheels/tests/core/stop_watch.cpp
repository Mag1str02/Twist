#include <wheels/core/stop_watch.hpp>

#include <wheels/test/framework.hpp>

#include <thread>

using wheels::StopWatch;

using namespace std::chrono_literals;
using std::this_thread::sleep_for;

TEST_SUITE(StopWatch) {
  SIMPLE_TEST(Elapsed) {
    StopWatch stop_watch;
    sleep_for(100ms);
    ASSERT_TRUE(stop_watch.Elapsed() >= 100ms);
  }

  SIMPLE_TEST(Restart) {
    static const auto kPeriod = 100ms;

    StopWatch stop_watch;

    sleep_for(kPeriod);
    auto elapsed = stop_watch.Restart();
    ASSERT_TRUE(elapsed >= kPeriod);

    sleep_for(kPeriod);
    elapsed = stop_watch.Elapsed();
    ASSERT_TRUE(elapsed >= kPeriod);
    ASSERT_TRUE(elapsed <= kPeriod * 2);
  }
}
