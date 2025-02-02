#include <wheels/support/progress.hpp>

#include <wheels/test/framework.hpp>

#include <thread>

TEST_SUITE(Progress) {
  SIMPLE_TEST(Bar) {
    wheels::ProgressBar progress("Steps");

    static const size_t kSteps = 10'000'000;

    progress.Start(kSteps);
    for (size_t i = 0; i < kSteps; ++i) {
      progress.MakeProgress();
      std::this_thread::yield();
    }
  }

  SIMPLE_TEST(Percents) {
    wheels::ProgressBar progress("Steps",
      wheels::ProgressBar::Options{false, '#', 50, false});

    static const size_t kSteps = 10'000'000;

    progress.Start(kSteps);
    for (size_t i = 0; i < kSteps; ++i) {
      progress.MakeProgress();
      std::this_thread::yield();
    }
  }
}
