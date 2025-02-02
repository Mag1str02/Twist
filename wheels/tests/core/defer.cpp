#include <wheels/core/defer.hpp>

#include <wheels/test/framework.hpp>

using wheels::Defer;

TEST_SUITE(Defer) {
  SIMPLE_TEST(One) {
    int a = 0;
    {
      Defer defer([&]() { a += 1; });
      ASSERT_EQ(a, 0);
    }
    ASSERT_EQ(a, 1);
  }

  SIMPLE_TEST(Many) {
    int a = 0;
    {
      Defer defer1([&]() { a += 1; });
      Defer defer2([&]() { a += 2; });
      ASSERT_EQ(a, 0);
    }
    ASSERT_EQ(a, 3);
  }

  SIMPLE_TEST(Nested) {
    int a = 0;
    {
      Defer defer([&]() { a += 1; });
      {
        Defer defer([&]() { a += 2; });
        ASSERT_EQ(a, 0);
      }
      ASSERT_EQ(a, 2);
    }
    ASSERT_EQ(a, 3);
  }
}
