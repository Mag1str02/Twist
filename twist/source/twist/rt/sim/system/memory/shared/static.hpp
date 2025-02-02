#pragma once

#include "stack_allocator.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

StackAllocator* StaticStackAllocator();

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
