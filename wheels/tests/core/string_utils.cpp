#include <wheels/core/string_utils.hpp>

#include <wheels/test/framework.hpp>

#include <sstream>

TEST_SUITE(StringUtils) {
  SIMPLE_TEST(Split) {
    auto tokens = wheels::Split("1,22,333", ',');
    ASSERT_EQ(tokens.size(), 3);
    ASSERT_EQ(tokens[0], "1");
    ASSERT_EQ(tokens[1], "22");
    ASSERT_EQ(tokens[2], "333");
  }

  SIMPLE_TEST(SplitOneToken) {
    auto tokens = wheels::Split("Hello", '!');
    ASSERT_EQ(tokens.size(), 1);
    ASSERT_EQ(tokens[0], "Hello");
  }

  SIMPLE_TEST(SplitEmptyTokens) {
    auto tokens = wheels::Split(",1,,2,", ',');
    ASSERT_EQ(tokens.size(), 5);
    ASSERT_EQ(tokens[0], "");
    ASSERT_EQ(tokens[1], "1");
    ASSERT_EQ(tokens[2], "");
    ASSERT_EQ(tokens[3], "2");
    ASSERT_EQ(tokens[4], "");
  }

  SIMPLE_TEST(QuotedString) {
    std::stringstream out;
    out << wheels::Quoted(std::string("Hello!"));
    ASSERT_EQ(out.str(), "'Hello!'");
  }

  SIMPLE_TEST(QuotedInt) {
    std::stringstream out;
    out << wheels::Quoted(42);
    ASSERT_EQ(out.str(), "'42'");
  }
}
