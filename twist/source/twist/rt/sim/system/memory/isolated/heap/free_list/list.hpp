#pragma once

#include "../brk.hpp"
#include "../context.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

// Intrusive singly-linked stack of blocks

class BlockLinkedFreeList {
 public:
  void Push(Block* item) noexcept {
    item->next = head_;
    head_ = item;
  }

  Block* TryPop() noexcept {
    if (head_ == nullptr) {
      return nullptr;  // Empty
    }

    Block* item = head_;
    head_ = head_->next;
    return item;
  }

  Block* TryPop(AllocContext /*ignore*/) {
    return TryPop();
  }

  static constexpr bool kSupportsRandomization = false;

  bool IsEmpty() const noexcept {
    return head_ == nullptr;
  }

  bool NonEmpty() const noexcept {
    return !IsEmpty();
  }

  void Reset(BumpAllocator*) {
    head_ = nullptr;
  }

 private:
  Block* head_ = nullptr;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
