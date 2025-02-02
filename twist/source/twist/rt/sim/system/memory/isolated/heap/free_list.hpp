#pragma once

// Implementation
// #include "free_list/list.hpp"
#include "free_list/vector.hpp"

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

using BlockFreeList = BlockVectorFreeList;

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
