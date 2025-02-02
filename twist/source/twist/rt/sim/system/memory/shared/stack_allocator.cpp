#include "stack_allocator.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

StackAllocator::StackAllocator() {
  Warmup();
}

void StackAllocator::Reset() {
}

Stack StackAllocator::Reuse(Stack stack) {
  // Cleanup?
  return stack;
}

Stack StackAllocator::Allocate() {
  if (!pool_.empty()) {
    Stack stack = std::move(pool_.back());
    pool_.pop_back();
    return Reuse(std::move(stack));
  }
  return AllocateNew();
}

Stack StackAllocator::AllocateNew() {
  return Stack::AllocateBytes(/*at_least=*/stack_size_);
}

void StackAllocator::Release(Stack stack) {
  pool_.push_back(std::move(stack));
}

void StackAllocator::SetMinStackSize(size_t bytes) {
  if (bytes > stack_size_) {
    pool_.clear();
  }
  stack_size_ = bytes;
}

void StackAllocator::Warmup() {
  static const size_t kWarmupSize = 8;

  pool_.reserve(kWarmupSize);
  for (size_t i = 0; i < kWarmupSize; ++i) {
    pool_.push_back(AllocateNew());
  }
}

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
