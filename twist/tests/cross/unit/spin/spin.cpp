#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/thread.hpp>

#include <twist/ed/wait/spin.hpp>

#include <wheels/core/compiler.hpp>

using twist::ed::std::thread;

using twist::ed::SpinWait;

namespace this_thread = twist::ed::std::this_thread;

using namespace std::chrono_literals;


TEST_SUITE(SpinWait) {
  SIMPLE_TEST(JustWorks) {
    twist::cross::Run([] {
      SpinWait spin_wait;

      // operator()
      spin_wait();

      spin_wait.Spin();
    });
  }

  SIMPLE_TEST(Spin1234) {
    twist::cross::Run([] {
      SpinWait spin_wait;

      for (size_t i = 0; i < 1234; ++i) {
        spin_wait();
      }
    });
  }

  SIMPLE_TEST(ConsiderParking) {
    twist::cross::Run([] {
      SpinWait spin_wait;

      ASSERT_FALSE(spin_wait.ConsiderParking());
      ASSERT_TRUE(spin_wait.KeepSpinning());

      while (spin_wait.KeepSpinning()) {
        spin_wait();
      }

      ASSERT_TRUE(spin_wait.ConsiderParking());
    });
  }
}
