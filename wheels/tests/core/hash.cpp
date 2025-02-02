#include <wheels/core/hash.hpp>
#include <wheels/memory/view_of.hpp>

#include <wheels/test/framework.hpp>

#include <iostream>

using wheels::ViewOf;
using wheels::HashRange;

TEST_SUITE(Hash) {
  SIMPLE_TEST(HashRange) {
    static const char* kMessage1 = "Hello, world!";
    static const char* kMessage2 = "Hell0, world!";

    size_t h1 = HashRange(13, ViewOf(kMessage1));
    size_t h2 = HashRange(13, ViewOf(kMessage2));
    size_t h3 = HashRange(13, ViewOf(kMessage1));
    size_t h4 = HashRange(12, ViewOf(kMessage2));

    ASSERT_EQ(h1, h3);
    ASSERT_NE(h1, h2);
    ASSERT_NE(h2, h4);
  }
}
