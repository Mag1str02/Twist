#pragma once

#include "stack.hpp"

#include <vector>

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

class StackAllocator {
  static constexpr size_t kDefaultStackSize = 2 * 1024 * 1024;

 public:
  StackAllocator();

  void Reset();

  Stack Allocate();
  void Release(Stack);

  void SetMinStackSize(size_t bytes);

 private:
  Stack Reuse(Stack);
  Stack AllocateNew();
  void Warmup();

 private:
  std::vector<Stack> pool_;
  size_t stack_size_ = kDefaultStackSize;
};

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
