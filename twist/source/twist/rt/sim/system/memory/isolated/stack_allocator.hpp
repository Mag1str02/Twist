#pragma once

#include "shared_segment.hpp"
#include "stack.hpp"
#include "panic.hpp"

#include <algorithm>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

class StackAllocator {
  struct StackNode {
    StackNode* next = nullptr;
  };

 public:
  StackAllocator(SharedSegment* segment, size_t min_stack_size)
    : segment_(segment), stack_size_(ChooseStackSize(min_stack_size)) {
  }

  // Precondition: before first allocation
  void SetMinStackSize(size_t min_size) {
    stack_size_ = ChooseStackSize(min_size);
  }

  void HardReset() {
    pool_ = nullptr;
  }

  std::optional<Stack> Allocate() {
    if (pool_ != nullptr) {
      // Take from pool
      StackNode* stack = pool_;
      pool_ = pool_->next;
      return Stack{(std::byte*)stack, stack_size_};
    }

    // Try to allocate new
    if (char* stack = segment_->TryDown(stack_size_)) {
      return Stack{(std::byte*)stack, stack_size_};
    }

    return std::nullopt;  // Overflow
  }

  void Free(Stack stack) {
    // Add to pool
    StackNode* stack_node = (StackNode*)stack.addr;
    stack_node->next = pool_;
    pool_ = stack_node;
  }

 private:
  static size_t ChooseStackSize(size_t min_size) {
    static const size_t kMinStackSize = 64 * 1024;
    return std::max(min_size, (size_t)kMinStackSize);
  }

 private:
  SharedSegment* segment_;
  size_t stack_size_;
  StackNode* pool_ = nullptr;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
