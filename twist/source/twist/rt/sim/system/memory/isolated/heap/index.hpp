#pragma once

#include "block.hpp"

#include <cstdint>
#include <cstdlib>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

class BlockIndex {
 public:
  static constexpr size_t kCapacity = 1024 * 1024;
  static constexpr size_t kSizeBytes = kCapacity * sizeof(Block*);

 public:
  void Reset(void* index) {
    blocks_ = (Block**)index;
    size_ = 0;
  }

  void Insert(Block* next) {
    WHEELS_VERIFY(size_ < kCapacity, "Block index overflow");
    if (!IsEmpty()) {
      WHEELS_VERIFY(next > Last(), "Unordered insert to block index");
    }
    blocks_[size_++] = next;
  }

  Block* Lookup(void* user_addr) const {
    if (IsEmpty()) {
      return nullptr;
    }

    {
      // Bounds

      if (user_addr < First()->Data()) {
        return nullptr;
      }

      if (user_addr >= Last()->UserDataEnd()) {
        return nullptr;
      }
    }

    // Find last block which address < user_addr
    Block** iter = std::lower_bound(blocks_, blocks_ + size_, user_addr);
    WHEELS_VERIFY(iter != blocks_, "Broken block index");
    --iter;

    {
      Block* candidate = *iter;
      if ((user_addr >= candidate->Data()) && (user_addr < candidate->UserDataEnd())) {
        return candidate;
      } else {
        return nullptr;
      }
    }
  }

 private:
  bool IsEmpty() const {
    return size_ == 0;
  }

  // Precondition: !IsEmpty()
  Block* First() const {
    return blocks_[0];
  }

  // Precondition: !IsEmpty()
  Block* Last() const {
    return blocks_[size_ - 1];
  }

 private:
  Block** blocks_;
  size_t size_ = 0;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
