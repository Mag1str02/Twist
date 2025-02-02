#pragma once

#include <cstdint>

namespace twist::rt::sim {

namespace system {

struct SchedulerThreadContext {
  uintptr_t f1;
  uintptr_t f2;
  uintptr_t f3;
  uintptr_t f4;
};

}  // namespace system

}  // namespace twist::rt::sim
