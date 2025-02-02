#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/assist/random.hpp>
#include <twist/test/either.hpp>

TEST_SUITE(Random) {
  SIMPLE_TEST(Choice1) {
    twist::cross::Run([] {
      twist::ed::std::random_device rd;
      twist::assist::Choice choice{rd};

      for (size_t i = 0; i < 1000; ++i) {
        size_t k = choice(13);
        ASSERT_TRUE(k < 13);
      }
    });
  }

  SIMPLE_TEST(Choice2) {
    twist::cross::Run([] {
      twist::ed::std::random_device rd;
      twist::assist::Choice choice{rd};

      for (size_t i = 0; i < 10; ++i) {
        while (choice(10) != i) {
          // Try again
        }
      }
    });
  }

  SIMPLE_TEST(Either) {
    twist::cross::Run([] {
      {
        // Wait false
        while (twist::test::Either()) {
          // Try again
        }
      }

      {
        // Wait true
        while (!twist::test::Either()) {
          // Try again
        }
      }
    });
  }
}
