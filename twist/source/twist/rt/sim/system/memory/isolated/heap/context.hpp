#pragma once

#include <cstdint>
#include <optional>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

struct AllocContext {
  std::optional<uint64_t> random;
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
