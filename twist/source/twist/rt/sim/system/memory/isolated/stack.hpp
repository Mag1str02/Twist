#pragma once

#include <sure/stack_view.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

// Heap-allocated stack without guard page

struct Stack {
  sure::StackView MutView() {
    return {addr, size};
  }

  std::byte* addr;
  size_t size;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
