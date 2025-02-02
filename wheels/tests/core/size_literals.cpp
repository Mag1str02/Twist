#include <wheels/core/size_literals.hpp>

#include <wheels/test/framework.hpp>

using namespace wheels::size_literals;

TEST_SUITE(SizeLiterals) {
  SIMPLE_TEST(CountBytes) {
    ASSERT_EQ(4_KiB, 4 * 1024);
    ASSERT_EQ(16_MiB, 16 * 1024 * 1024);
    ASSERT_EQ(42_GiB, (size_t)42 * 1024 * 1024 * 1024);
  }
}
