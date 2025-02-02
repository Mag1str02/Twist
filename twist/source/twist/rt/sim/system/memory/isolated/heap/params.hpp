#pragma once

#include <optional>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

struct AllocatorParams {
  bool zero_memory;
  std::optional<unsigned char> memset;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
