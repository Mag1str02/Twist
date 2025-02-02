#include <wheels/core/noncopyable.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/core/compiler.hpp>

#include <type_traits>

struct Test : private wheels::NonCopyable {
};

TEST_SUITE(NonCopyable) {
  SIMPLE_TEST(Works) {
    Test test{};  // Ok
    WHEELS_UNUSED(test);

    static_assert(!std::is_copy_constructible<Test>::value, "Broken NonCopyable");
    static_assert(!std::is_copy_assignable<Test>::value, "Broken NonCopyable");
  }

  SIMPLE_TEST(NonConstructible) {
    static_assert(!std::is_constructible<wheels::NonCopyable>::value, "Broken NonCopyable");
  }
}
