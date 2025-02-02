#pragma once

#include "../brk.hpp"
#include "../context.hpp"

// std::memcpy
#include <cstring>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

class BlockVectorFreeList {
 public:
  BlockVectorFreeList() = default;

  void Push(Block* block) {
    if (size_ == capacity_) {
      ResizeBuffer();
    }

    buf_[size_] = block;
    ++size_;
  }

  Block* TryPopBack() {
    if (size_ > 0) {
      --size_;
      return buf_[size_];
    } else {
      return nullptr;
    }
  }

  Block* TryPopSome(uint64_t random) {
    if (size_ == 0) {
      return nullptr;
    }

    size_t index = random % size_;
    std::swap(buf_[index], buf_[size_ - 1]);

    {
      --size_;
      return buf_[size_];
    }
  }

  Block* TryPop(AllocContext context) {
    if (context.random) {
      return TryPopSome(*context.random);
    } else {
      return TryPopBack();
    }
  }

  static constexpr bool kSupportsRandomization = true;

  void Reset(BrkAllocator* blocks) {
    blocks_ = blocks;

    buf_ = nullptr;
    size_ = 0;
    capacity_ = 0;
  }

 private:
  static size_t NewCapacity(size_t curr) {
    if (curr == 0) {
      return 8;  // Initial
    } else {
      return curr * 2;
    }
  }

  static size_t ToBytes(size_t items) {
    return items * sizeof(Block*);
  }

  Block** NewBuffer(size_t capacity) {
    size_t bytes = ToBytes(capacity);
    size_t klass = SizeClassifier::GetSizeClass(bytes);

    Block* block = blocks_->NewBlock(klass);

    block->header.state = BlockState::Allocated;
    block->header.service = true;

    return (Block**)block->Data();
  }

  void ResizeBuffer() {
    size_t new_capacity = NewCapacity(/*curr=*/capacity_);

    Block** new_buf = NewBuffer(new_capacity);

    if (buf_ != nullptr) {
      // Copy data
      std::memcpy(new_buf, buf_, ToBytes(size_));
    }

    // Leak current buffer to prevent cascading ResizeBuffer-s
    buf_ = new_buf;
    capacity_ = new_capacity;
  }

 private:
  // For service allocations
  BrkAllocator* blocks_;

  Block** buf_ = nullptr;
  size_t capacity_ = 0;
  size_t size_ = 0;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
