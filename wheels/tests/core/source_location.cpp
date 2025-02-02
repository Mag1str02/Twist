#include <wheels/test/framework.hpp>

using wheels::SourceLocation;

TEST_SUITE(SourceLocation) {
  SIMPLE_TEST(Here) {
    ASSERT_EQ(wheels::Here().Line(), 7);

    std::string_view this_file = wheels::Here().File();
    ASSERT_TRUE(this_file.ends_with("/source_location.cpp"));
  }

  SIMPLE_TEST(OutOperator) {
    std::cout << "Here = " << wheels::Here() << std::endl;
  }
}
