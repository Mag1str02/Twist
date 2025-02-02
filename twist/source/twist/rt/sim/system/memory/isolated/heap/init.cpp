#include "init.hpp"

#include "../alignment.hpp"
#include "../panic.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

// Allocate memory for allocator itself
char* AllocateBootBlock(SharedSegment* segment, size_t size) {
  char* start = segment->UpPtr();

  char* boot_block_start = Align(start);

  size_t boot_block_size = 16;
  while (boot_block_size < size) {
    boot_block_size *= 2;
  }

  if (!segment->TrySetUp(boot_block_start + boot_block_size)) {
    HardPanic("Cannot bootstrap malloc allocator", wheels::SourceLocation::Current());
  }

  return boot_block_start;
}

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
