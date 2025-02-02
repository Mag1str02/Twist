#pragma once

#include "stat/futex.hpp"

namespace twist::sim {

// Backward compatibility

using stat::FutexWaitSystemCallCount;
using stat::FutexWakeSystemCallCount;
using stat::FutexSystemCallCount;

}  // namespace twist::sim
