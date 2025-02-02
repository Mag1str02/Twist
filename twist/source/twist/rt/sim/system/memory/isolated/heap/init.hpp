#pragma once

#include "../shared_segment.hpp"

#include <cstdint>
#include <cstdlib>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

// Allocate memory for allocator itself
char* AllocateBootBlock(SharedSegment* segment, size_t size);

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
