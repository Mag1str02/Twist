#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/chrono.hpp>

#include <wheels/core/compiler.hpp>

using twist::ed::std::chrono::system_clock;
using twist::ed::std::chrono::steady_clock;

TEST_SUITE(StdChrono) {
  SIMPLE_TEST(SystemClock) {
    twist::cross::Run([] {
      system_clock::time_point now = system_clock::now();
      WHEELS_UNUSED(now);
    });
  }

  SIMPLE_TEST(SteadyClock) {
    twist::cross::Run([] {
      steady_clock::time_point now = steady_clock::now();
      WHEELS_UNUSED(now);
    });
  }
}
