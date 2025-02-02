#pragma once

#include <cstddef>

namespace twist::sim {

namespace stat {

size_t AllocationCount();

size_t TotalBytesAllocated();

size_t WorkingSetInBytes();

}  // namespace stat

}  // namespace twist::sim
