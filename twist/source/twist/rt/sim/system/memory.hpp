#pragma once

#if defined(__TWIST_SIM_ISOLATION__)

#include "memory/isolated/memory.hpp"
#include "memory/isolated/guard.hpp"

namespace twist::rt::sim {

namespace system {

// User memory isolated from system memory
using memory::isolated::Memory;

using memory::isolated::Stack;

using memory::isolated::UserAllocGuard;

}  // namespace system

}  // namespace twist::rt::sim

#else

#include "memory/shared/memory.hpp"
#include "memory/shared/guard.hpp"

namespace twist::rt::sim {

namespace system {

// Memory shared between user and simulator (kernel)
using memory::shared::Memory;

using memory::shared::Stack;

using memory::shared::UserAllocGuard;

}  // namespace system

}  // namespace twist::rt::sim

#endif
