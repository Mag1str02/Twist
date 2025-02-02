#pragma once

#include "../alignment.hpp"
#include "../shared_segment.hpp"
#include "../panic.hpp"

#include "params.hpp"
#include "block.hpp"
#include "index.hpp"

#include <cassert>
// std::memset
#include <cstring>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

class BrkAllocator {
  // To simplify implementation
  static_assert(sizeof(BlockHeader) == kAlignment);

  static constexpr uint32_t kCanary = 0xAAAAAAAA;

  static constexpr size_t kHeaderSize = sizeof(BlockHeader);

 public:
  BrkAllocator(SharedSegment* segment, AllocatorParams params)
      : params_(params),
        segment_(segment) {
    Reset();
  }

  Block* NewBlock(SizeClass klass) {
    ___TWIST_ALLOCATOR_VERIFY(IsAligned(next_), "Invalid alignment");

    // payload
    size_t klass_block_size = SizeClassifier::BlockSize(klass);
    // block header + payload
    size_t total_block_size = klass_block_size + kHeaderSize;
    // + _next_ block header
    size_t reserve_bytes = total_block_size + kHeaderSize;

    if (reserve_bytes > BytesLeft()) {
      return nullptr;  // Overflow
    }

    Block* block = (Block*)next_;
    char* new_next = Align(next_ + total_block_size);

    if (!segment_->TrySetUp(new_next)) {
      return nullptr;  // Overflow
    }

    next_ = new_next;

    Init(block, klass);

    index_.Insert(block);

    return block;
  }

  bool FromHere(char* ptr) const {
    return (ptr >= segment_->Begin()) && (ptr < segment_->UpPtr());
  }

  Block* GetFreedBlock(char* ptr) const {
    ___TWIST_ALLOCATOR_VERIFY(FromHere(ptr),
                              "Invalid user pointer: not from heap");
    ___TWIST_ALLOCATOR_VERIFY(IsAligned(ptr),
                              "Invalid user pointer: unaligned");
    ___TWIST_ALLOCATOR_VERIFY(ptr >= segment_->Begin() + sizeof(BlockHeader),
                              "Invalid user pointer");
    ___TWIST_ALLOCATOR_VERIFY(ptr < next_,
                              "Invalid user pointer: not allocated");

    Block* block = (Block*)(ptr - sizeof(BlockHeader));

    ___TWIST_ALLOCATOR_VERIFY(block->Canary() == kCanary,
                              "Corrupted memory: block canary is dead");

    return block;
  }

  Block* Lookup(char* addr) const {
    return index_.Lookup(addr);
  }

  void Reset() {
    {
      char* index = Align(segment_->UpPtr());
      ___TWIST_ALLOCATOR_VERIFY(segment_->TrySetUp(index), "Broken allocator");
      ___TWIST_ALLOCATOR_VERIFY(segment_->TryUp(BlockIndex::kSizeBytes),
                                "Cannot reserve space for index");
      index_.Reset(index);
    }

    next_ = Align(segment_->UpPtr());
    ___TWIST_ALLOCATOR_VERIFY(segment_->TrySetUp(next_), "Broken allocator");
    std::memset(next_, 0, kHeaderSize);
    ___TWIST_ALLOCATOR_VERIFY(IsAligned(next_), "Invalid alignment");
  }

 private:
  size_t BytesLeft() const {
    return segment_->SpaceLeft();
  }

  void Init(Block* block, SizeClass klass) {
    block->header.klass = klass;
    block->header.canary = kCanary;
    if (params_.zero_memory) {
      // Fill with zeroes
      memset(block->Data(), 0, block->Size());
    }
    memset(next_, 0, kHeaderSize);
  }

 private:
  const AllocatorParams params_;
  SharedSegment* segment_;
  BlockIndex index_;
  char* next_;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim