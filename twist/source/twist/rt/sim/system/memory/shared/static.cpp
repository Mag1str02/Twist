#include "static.hpp"

#include <wheels/core/panic.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

#if defined(__TWIST_SIM__) && !defined(__TWIST_SIM_ISOLATION__)

static StackAllocator stack_allocator;

StackAllocator* StaticStackAllocator() {
  return &stack_allocator;
}

#else

StackAllocator* StaticStackAllocator() {
  WHEELS_PANIC("Not supported in this configuration");
}

#endif

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
