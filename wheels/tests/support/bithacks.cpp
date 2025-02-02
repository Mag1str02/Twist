#include <wheels/support/bithacks.hpp>

#include <wheels/test/framework.hpp>

#include <random>

TEST_SUITE(Bits) {
  SIMPLE_TEST(IsPowerOfTwo) {
    for (size_t i = 0; i < 30; ++i) {
      ASSERT_TRUE(IsPowerOfTwo(1u << i));
    }

    ASSERT_FALSE(IsPowerOfTwo(3));
    ASSERT_FALSE(IsPowerOfTwo(42));
    ASSERT_FALSE(IsPowerOfTwo(1344100001));
  }

  SIMPLE_TEST(GetBit) {
    static const size_t kNumber = 103701;  // 11001010100010101 (17 digits)

    ASSERT_EQ(GetBit(kNumber, 0), 1);
    ASSERT_EQ(GetBit(kNumber, 1), 0);
    ASSERT_EQ(GetBit(kNumber, 2), 1);
    ASSERT_EQ(GetBit(kNumber, 3), 0);
    ASSERT_EQ(GetBit(kNumber, 7), 0);
    ASSERT_EQ(GetBit(kNumber, 8), 1);
    ASSERT_EQ(GetBit(kNumber, 14), 0);
    ASSERT_EQ(GetBit(kNumber, 16), 1);
  }

  SIMPLE_TEST(GetBitCornerCases) {
    ASSERT_EQ(GetBit((size_t)1u << 63, 63), 1);
    ASSERT_EQ(GetBit(0, 63), 0);
  }

  SIMPLE_TEST(MostSignificantBitCornerCases) {
    ASSERT_EQ(MostSignificantBit(0), 0);
    ASSERT_EQ(MostSignificantBit((size_t)1u << 63), (size_t)1u << 63);
  }

  SIMPLE_TEST(MostSignificantBitRandomCases) {
    static const size_t kTrials = 1000;
    static const size_t kWidth = 64;

    std::mt19937_64 twister{42};

    for (size_t i = 0; i < kTrials; ++i) {
      size_t number = twister();
      for (int k = kWidth - 1; k > 0; --k) {
        if (GetBit(number, k) == 1) {
          ASSERT_EQ(MostSignificantBit(number), (size_t)1u << k);
          break;
        }
      }
    }
  }
}
