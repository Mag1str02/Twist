#pragma once

#include "static.hpp"

#include <twist/rt/sim/system/params.hpp>

#include <cstdlib>
#include <cstring>
#include <vector>

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

class Memory {
 public:
  Memory(const Params& /*ignored*/) {
  }

  void Reset() {
    StaticStackAllocator()->Reset();
  }

  // Heap

  void InitMalloc() {
    // No-op
  }

  void* Malloc(size_t size) {
    return std::malloc(size);
  }

  void Free(void* ptr) {
    std::free(ptr);
  }

  bool Access(void* /*ptr*/, size_t /*size*/) {
    return true;  // No-op
  }

  // Stacks

  void SetMinStackSize(size_t bytes) {
    StaticStackAllocator()->SetMinStackSize(bytes);
  }

  Stack AllocateStack() {
    return StaticStackAllocator()->Allocate();
  }

  void FreeStack(Stack stack) {
    StaticStackAllocator()->Release(std::move(stack));
  }

  // Static variables

  void* AllocateStatic(size_t size) {
    void* addr = std::malloc(size);
    std::memset(addr, 0, size);
    statics_.push_back(addr);
    return addr;
  }

  void DeallocateStatics() {
    for (void* addr : statics_) {
      std::free(addr);
    }
    statics_.clear();
  }

 private:
  std::vector<void*> statics_;
};

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
