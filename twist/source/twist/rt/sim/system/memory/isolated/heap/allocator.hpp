#pragma once

#include "../shared_segment.hpp"
#include "../panic.hpp"

#include "brk.hpp"
#include "context.hpp"
#include "free_list.hpp"
#include "stat.hpp"
#include "params.hpp"

// std::memset
#include <cstring>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

class Allocator {
  static constexpr size_t kClasses = 32;

  static const size_t kAllocSizeLimit = 1 << 30;

 public:
  Allocator(SharedSegment* segment, AllocatorParams params)
      : params_(params),
        brk_allocator_(segment, params) {
  }

  static constexpr bool kSupportsRandomization =
      BlockFreeList::kSupportsRandomization;

  const AllocatorParams& Params() const {
    return params_;
  }

  void* Allocate(size_t user_size,
                                  AllocContext context = AllocContext{}) {
    ___TWIST_SIM_USER_VERIFY(user_size < kAllocSizeLimit,
                               "Allocation is too large");

    SizeClass klass = SizeClassifier::GetSizeClass(user_size);

    Block* block = GetFreeBlock(klass, context);

    if (block == nullptr) {
      return nullptr;  // Overflow
    }

    {
      // Set metadata
      block->header.state = BlockState::Allocated;
      block->header.service = false;
      block->header.user_size = user_size;
      block->header.alloc_id = ++next_id_;

      Fill(block);
    }

    {
      // Statistics
      ++stat_.alloc_count;
      stat_.bytes_allocated += block->Size();
      stat_.user_bytes_allocated += user_size;
    }

    return block->Data();
  }

  void Free(char* ptr) {
    if (ptr == nullptr) {
      return;  // delete nullptr
    }

    Free(brk_allocator_.GetFreedBlock(ptr));
  }

  bool Access(char* addr, size_t size) const {
    Block* block = brk_allocator_.Lookup(addr);
    if (block == nullptr) {
      return false;
    } else {
      return IsAllocated(block) && (addr + size <= block->UserDataEnd());
    }
  }

  void Reset() {
    brk_allocator_.Reset();

    // Reset free lists
    for (size_t k = 0; k < kClasses; ++k) {
      free_lists_[k].Reset(&brk_allocator_);
    }

    next_id_ = 0;

    {
      // Reset statistics

      stat_.bytes_allocated = 0;
      stat_.bytes_freed = 0;

      stat_.user_bytes_allocated = 0;
      stat_.user_bytes_freed = 0;

      stat_.alloc_count = 0;
      stat_.free_count = 0;
    }
  }

  // For checks
  bool FromHere(void* ptr) const {
    return brk_allocator_.FromHere((char*)ptr);
  }

  AllocatorStat Stat() const {
    return stat_;
  }

 private:
  Block* GetFreeBlock(SizeClass klass, AllocContext context) {
    if (Block* block = free_lists_[klass].TryPop(context); block != nullptr) {
      return block;
    }
    return brk_allocator_.NewBlock(klass);
  }

  void Free(Block* block) {
    if (!IsAllocated(block)) {
      user::Panic(system::Status::MemoryDoubleFree,
                  "Corrupted user memory: block already freed");
    }

    SizeClass klass = block->Class();
    ___TWIST_ALLOCATOR_VERIFY(klass < kClasses,
                              "Corrupted user memory: corrupted block header");

    {
      // Statistics
      ++stat_.free_count;
      stat_.user_bytes_freed += block->UserSize();
      stat_.bytes_freed += block->Size();
    }

    block->header.state = BlockState::Available;
    block->header.user_size = 0;

    {
      // Fill block with ones
      // memset(block->Data(), 255, block->Size());
    }

    free_lists_[klass].Push(block);
  }

  void Fill(Block* user) {
    if (params_.memset) {
      memset(user->Data(), *params_.memset, user->Size());
    } else {
      // Do nothing
    }
  }

  static bool IsAllocated(Block* block) {
    return block->header.state == BlockState::Allocated;
  }

 private:
  const AllocatorParams params_;
  BrkAllocator brk_allocator_;
  BlockFreeList free_lists_[kClasses];
  AllocId next_id_ = 0;
  AllocatorStat stat_;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
