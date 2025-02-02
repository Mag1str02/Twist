#pragma once

#include <cstdlib>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

struct AllocatorStat {
  size_t bytes_allocated = 0;
  size_t bytes_freed = 0;

  size_t user_bytes_allocated = 0;
  size_t user_bytes_freed = 0;

  size_t alloc_count = 0;
  size_t free_count = 0;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
