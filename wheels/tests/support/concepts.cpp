#include <wheels/support/concepts.hpp>

#include <wheels/test/framework.hpp>

#include <string>

TEST_SUITE(Concepts) {
  template <typename T>
  struct Widget {};

  struct Gadget {};

  SIMPLE_TEST(InstantiationOf) {
    using X = Widget<int>;
    using Y = Widget<std::string>;

    ASSERT_TRUE(true);

    std::cout << wheels::detail::IsInstance<X, Widget>::value << std::endl;
    std::cout << wheels::detail::IsInstance<Y, Widget>::value << std::endl;
    std::cout << wheels::detail::IsInstance<Gadget, Widget>::value << std::endl;
  }
}
