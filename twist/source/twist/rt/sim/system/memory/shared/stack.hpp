#pragma once

#include <sure/stack.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

// Mmap-ed stack with guard page
using Stack = sure::Stack;

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
