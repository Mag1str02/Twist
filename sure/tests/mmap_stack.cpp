#include <sure/stack/mmap.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MmapStack") {
  SECTION("AllocateBytes1") {
    size_t page_size = wheels::MmapAllocation::PageSize();

    auto stack = sure::MmapStack::AllocateBytes(/*at_least*/1);
    REQUIRE(stack.AllocationSize() == page_size * 2);
  }

  SECTION("AllocateBytes2") {
    size_t page_size = wheels::MmapAllocation::PageSize();

    auto stack = sure::MmapStack::AllocateBytes(/*at_least*/page_size * 3 + 1);
    REQUIRE(stack.AllocationSize() == page_size * 5);
  }
}
