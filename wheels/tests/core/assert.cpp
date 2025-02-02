#include <wheels/core/assert.hpp>

#include <wheels/test/framework.hpp>

TEST_SUITE(Assert) {
  void Test(int x) {
    // NB: no warnings here in release build!
    WHEELS_ASSERT(x > 0, "x <= 0");
  }

  SIMPLE_TEST(NoWarnings) {
    Test(17);
  }

  SIMPLE_TEST(AssertRelease) {
#ifdef NDEBUG
    WHEELS_ASSERT(true == false, "Fail");
#endif
  }
}
