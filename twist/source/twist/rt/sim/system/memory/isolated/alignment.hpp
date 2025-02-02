#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstdint>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

//////////////////////////////////////////////////////////////////////

// Fundamental alignment
static inline const size_t kAlignment = 16;

static_assert(kAlignment >= alignof(std::max_align_t));

//////////////////////////////////////////////////////////////////////

inline bool IsAligned(char* ptr, size_t alignment = kAlignment) {
  return (std::uintptr_t)ptr % alignment == 0;
}

inline char* Align(char* ptr, size_t alignment = kAlignment) {
  while (!IsAligned(ptr, alignment)) {
    ++ptr;
  }
  return ptr;
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
