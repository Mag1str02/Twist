#pragma once

#include <cstddef>

namespace twist::sim {

namespace stat {

size_t FutexWaitSystemCallCount();
size_t FutexWakeSystemCallCount();

inline size_t FutexSystemCallCount() {
  return FutexWaitSystemCallCount() + FutexWakeSystemCallCount();
}

}  // namespace stat

}  // namespace twist::sim
