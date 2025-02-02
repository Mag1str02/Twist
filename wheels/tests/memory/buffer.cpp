#include <wheels/memory/buffer.hpp>

#include <wheels/test/framework.hpp>

#include <memory>

TEST_SUITE(GrowingBuffer) {
  // Helper
  std::string ReadAllFrom(const wheels::GrowingBuffer& buffer) {
    return std::string{buffer.View().Begin(), buffer.Size()};
  }

  SIMPLE_TEST(JustWorks) {
    wheels::GrowingBuffer buffer;

    buffer.Append("Hello");
    buffer.Append(", ");
    buffer.Append("World!");

    ASSERT_EQ(ReadAllFrom(buffer), "Hello, World!");
  }

  SIMPLE_TEST(Clear) {
    wheels::GrowingBuffer buffer;

    buffer.Append("Hello");
    buffer.Append("!");

    ASSERT_EQ(ReadAllFrom(buffer), "Hello!");

    buffer.Clear();

    buffer.Append("Test");
    buffer.Append("123");

    ASSERT_EQ(ReadAllFrom(buffer), "Test123");
  }
}