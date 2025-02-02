#include "memory.hpp"

#include "static.hpp"

#include "heap/init.hpp"

#include <twist/rt/sim/user/am_i_user.hpp>
#include <twist/rt/sim/user/syscall/random.hpp>

#include <wheels/core/assert.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

//////////////////////////////////////////////////////////////////////

static const size_t kDefaultStackSize = 2 * 1024 * 1024;

//////////////////////////////////////////////////////////////////////

static heap::AllocatorParams ToHeapAllocatorParams(const Params& params) {
  return {
      .zero_memory = params.zero_memory,
      .memset = params.memset_malloc
  };
}

Memory::Memory(const Params& params)
    : randomize_malloc_(params.randomize_malloc),
      segments_(StaticMemoryMapper()->MutView()),
      statics_(segments_.Static()),
      stacks_(segments_.Dynamic(), kDefaultStackSize),
      heap_params_(ToHeapAllocatorParams(params)) {
}

bool Memory::FixedMapping() const {
  return StaticMemoryMapper()->FixedAddress();
}

void Memory::Reset() {
  stacks_.HardReset();
  heap_ = nullptr;
}

void Memory::Burn() {
  segments_.Dynamic()->Reset();
  stacks_.HardReset();
  heap_ = nullptr;
}

void Memory::InitMalloc() {
  SharedSegment* dynamic = segments_.Dynamic();
  char* boot_block = heap::AllocateBootBlock(dynamic, sizeof(heap::Allocator));
  heap_ = new (boot_block) heap::Allocator{dynamic, heap_params_};
  heap_->Reset();
}

void* Memory::Malloc(size_t size) {
  WHEELS_ASSERT(user::AmIUser(), "User thread expected");
  void* addr = heap_->Allocate(size, HeapAllocContext());
  WHEELS_VERIFY(addr != nullptr, "Heap overflow");
  return addr;
}

void Memory::Free(void* p) {
  WHEELS_ASSERT(user::AmIUser(), "User thread expected");
  heap_->Free((char*)p);
}

bool Memory::Access(void* addr, size_t size) {
  WHEELS_ASSERT(user::AmIUser(), "User thread expected");
  return heap_->Access((char*)addr, size);
}

//

heap::AllocContext Memory::HeapAllocContext() {
  heap::AllocContext ctx;

  if constexpr (heap::Allocator::kSupportsRandomization) {
    if (randomize_malloc_) {
      ctx.random = user::syscall::RandomNumber();
    }
  }

  return ctx;
}

Stack Memory::AllocateStack() {
  auto stack = stacks_.Allocate();
  WHEELS_VERIFY(stack.has_value(), "Stack memory overflow");
  return std::move(*stack);
}

void Memory::FreeStack(Stack stack) {
  stacks_.Free(stack);
}

void Memory::SetMinStackSize(size_t bytes) {
  stacks_.SetMinStackSize(bytes);
}

void* Memory::AllocateStatic(size_t size) {
  void* addr = statics_.Allocate(size);
  WHEELS_VERIFY(addr != nullptr, "Static segment overflow");
  return addr;
}

void Memory::DeallocateStatics() {
  statics_.Reset();
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
