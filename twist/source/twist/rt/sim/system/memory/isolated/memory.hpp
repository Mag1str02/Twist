#pragma once

#include "segment_manager.hpp"

#include "statics_allocator.hpp"
#include "heap/allocator.hpp"
#include "stack_allocator.hpp"

#include "guard.hpp"

#include <twist/rt/sim/system/params.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

// Isolated user memory

class Memory {
 public:
  Memory(const Params& params);

  void Reset();
  void Burn();

  // Heap (user)

  void InitMalloc();
  void* Malloc(size_t size);
  void Free(void* ptr);

  bool Access(void* ptr, size_t size);

  // Stacks (system)

  void SetMinStackSize(size_t);
  Stack AllocateStack();
  void FreeStack(Stack stack);

  heap::AllocatorStat HeapStat() const {
    return heap_->Stat();
  }

  // Static variables (user)

  void* AllocateStatic(size_t size);
  void DeallocateStatics();

  //

  bool FixedMapping() const;

 private:
  heap::AllocContext HeapAllocContext();

 private:
  bool randomize_malloc_;

  SegmentManager segments_;

  StaticsAllocator statics_;

  // Heap grows up, stacks - down
  StackAllocator stacks_;

  const heap::AllocatorParams heap_params_;
  heap::Allocator* heap_ = nullptr;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
