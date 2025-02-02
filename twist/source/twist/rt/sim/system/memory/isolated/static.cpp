#include "static.hpp"

#if defined(__TWIST_SIM_ISOLATION__)

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

static MemoryMapper mapper;

MemoryMapper* StaticMemoryMapper() {
  return &mapper;
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim

#else

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

MemoryMapper* StaticMemoryMapper() {
  WHEELS_PANIC("Not supported in this configuration");
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim

#endif
