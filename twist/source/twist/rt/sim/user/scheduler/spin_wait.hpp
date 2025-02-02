#pragma once

#include <cstddef>

namespace twist::rt::sim {

namespace user::scheduler {

// Yield threshold for SpinWait::ConsiderParking
size_t SpinWaitYieldThreshold();

}  // namespace scheduler

}  // namespace twist::rt::sim
