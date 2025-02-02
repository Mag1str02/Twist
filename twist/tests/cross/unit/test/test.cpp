#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/atomic.hpp>

#include <twist/test/choice.hpp>
#include <twist/test/either.hpp>
#include <twist/test/message.hpp>
#include <twist/test/lock_free.hpp>
#include <twist/test/checksum.hpp>

#include <optional>
#include <string>

TEST_SUITE(Test) {
  SIMPLE_TEST(ChoiceAlts) {
    twist::cross::Run([] {
      static constexpr size_t kAlts = 5;

      for (size_t i = 0; i < kAlts; ++i) {
        while (twist::test::Choice(kAlts) != i) {
          // Try again
        }
      }
    });
  }

  SIMPLE_TEST(ChoiceRange) {
    twist::cross::Run([] {
      static constexpr size_t kLo = 3;
      static constexpr size_t kHi = 7;

      for (size_t i = kLo; i < kHi; ++i) {
        while (twist::test::Choice(kLo, kHi) != i) {
          // Try again
        }
      }
    });
  }

  SIMPLE_TEST(Either) {
    twist::cross::Run([] {
      while (!twist::test::Either()) {
        // Try again
      }

      while (twist::test::Either()) {
        // Try again
      }
    });
  }

  SIMPLE_TEST(Message) {
    twist::cross::Run([] {
      using IntMessage = twist::test::Message<int>;

      std::optional<IntMessage> pipe;

      twist::ed::std::thread t([&] {
        pipe.emplace(IntMessage::New(1));
      });

      t.join();  // Synchronizes-with producer

      int v = pipe->Read();
      ASSERT_EQ(v, 1);
    });
  }

  SIMPLE_TEST(LockFree) {
    twist::cross::Run([] {
      {
        twist::test::LockFreeScope lf;

        twist::test::Progress();
      }
    });
  }

  SIMPLE_TEST(CheckSum) {
    twist::cross::Run([] {
      {
        twist::test::CheckSum<std::string> s;

        s.Produce("Hello");
        s.Produce("World");

        s.Consume("World");

        ASSERT_FALSE(s.Validate());

        s.Consume("Hello");

        ASSERT_TRUE(s.Validate());
      }
    });
  }
}
