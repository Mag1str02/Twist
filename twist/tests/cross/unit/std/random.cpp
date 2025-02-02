#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/random.hpp>

#include <wheels/core/compiler.hpp>

using twist::ed::std::random_device;

TEST_SUITE(StdRandomDevice) {
  SIMPLE_TEST(JustWorks) {
    twist::cross::Run([] {
      random_device d;

      {
        random_device::result_type v = d();
        WHEELS_UNUSED(v);
      }

      auto min = d.min();
      auto max = d.max();

      for (size_t i = 0; i < 15; ++i) {
        auto v = d();
        ASSERT_GE(v, min);
        ASSERT_LE(v, max);
      }

      WHEELS_UNUSED(d.entropy());
    });
  }

  SIMPLE_TEST(Wait7) {
    twist::cross::Run([] {
      twist::ed::std::random_device rd;
      std::uniform_int_distribution<int> d(1, 10);

      for (size_t i = 0; i < 1000; ++i) {
        if (d(rd) == 7) {
          return;
        }
      }

      WHEELS_UNREACHABLE();
    });
  }
}
