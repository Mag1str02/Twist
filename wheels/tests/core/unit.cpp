#include <wheels/core/unit.hpp>

#include <wheels/test/framework.hpp>

#include <vector>

TEST_SUITE(Unit) {
  SIMPLE_TEST(Container) {
    std::vector<wheels::Unit> units;
  }

  SIMPLE_TEST(Compare) {
    wheels::Unit u1;
    wheels::Unit u2;
    ASSERT_TRUE(u1 == u2);
    ASSERT_FALSE(u1 != u2);
  }
}
