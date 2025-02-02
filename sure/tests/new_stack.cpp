#include <sure/stack/new.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("NewStack") {
  SECTION("AllocateBytes") {
    auto stack = sure::NewStack::AllocateBytes(1024 * 1024);
    auto stack_view = stack.MutView();
    std::byte* bottom = stack_view.data() + stack_view.size();

    *(bottom - 100) = std::byte{0xFF};
  }

  SECTION("MoveConstructible") {
    auto stack1 = sure::NewStack::AllocateBytes(1024 * 1024);
    auto stack2 = std::move(stack1);
  }
}
