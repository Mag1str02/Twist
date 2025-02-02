#include <wheels/system/mmap.hpp>

#include <wheels/memory/view.hpp>
#include <wheels/memory/view_of.hpp>

#include <wheels/test/framework.hpp>

using wheels::MmapAllocation;
using wheels::MutableMemView;
using wheels::ConstMemView;
using wheels::ViewOf;
using wheels::MutViewOf;

TEST_SUITE(MemSpan) {
  SIMPLE_TEST(DefaultCtor) {
    MutableMemView mut_view{};
    ASSERT_FALSE(mut_view.IsValid());

    ConstMemView const_view{};
    ASSERT_FALSE(const_view.IsValid());
  }

  SIMPLE_TEST(Ops) {
    auto alloc = MmapAllocation::AllocatePages(1);

    ConstMemView const_view = alloc.View();

    ASSERT_TRUE(const_view.IsValid());

    ASSERT_EQ(const_view.Data(), alloc.Start());
    ASSERT_EQ(const_view.Size(), alloc.Size());
    ASSERT_EQ(const_view.Begin(), alloc.Start());
    ASSERT_FALSE(const_view.IsEmpty());
  }
}

TEST_SUITE(ViewOf) {
  SIMPLE_TEST(ViewOfArray) {
    static const size_t kLength = 15;
    char data[kLength];

    auto mut_view = wheels::MutViewOf(data);
    ASSERT_TRUE(mut_view.IsValid());
    ASSERT_EQ(mut_view.Data(), data);
    ASSERT_EQ(mut_view.Size(), kLength);
  }

  SIMPLE_TEST(Offset) {
    static const size_t kLength = 15;
    char data[kLength];

    auto mut_view = MutViewOf(data);

    size_t bytes_read = 0;
    while (mut_view.HasSpace()) {
      mut_view += 1;  // Read some
      bytes_read += 1;
    }

    ASSERT_EQ(bytes_read, kLength);
  }

  SIMPLE_TEST(ViewOfStaticString) {
    static const char* kMessage = "Hello, world!";

    auto message_view = ViewOf(kMessage);
    ASSERT_EQ(message_view.Size(), strlen(kMessage));
  }

  SIMPLE_TEST(ViewOfStdString) {
    static const std::string kMessage = "Hello, world!";

    auto message_view = ViewOf(kMessage);
    ASSERT_EQ(message_view.Size(), kMessage.length());

    std::string message_str(message_view.Data(), message_view.Size());
    ASSERT_EQ(message_str, kMessage);
  }
}
