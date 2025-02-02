#include <wheels/test/support/sync_output.hpp>

#include <wheels/test/framework.hpp>

using namespace wheels;

TEST_SUITE(SyncOutput) {
  SIMPLE_TEST(SyncCout) {
    SyncCout() << "Hello" << ", " << "World";
  }

  SIMPLE_TEST(SyncCerr) {
    SyncCerr();  // << "smth" leads to test failure
  }

  SIMPLE_TEST(String) {
    std::stringstream stream;
    SyncOutput out(stream);
    out << "Test" << 1 << 2 << 3;
    ASSERT_EQ(stream.str(), "Test123");
  }
}
