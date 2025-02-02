#pragma once

#include <cstdlib>

namespace twist::rt::sim {

namespace system {

namespace memory::shared {

struct UserAllocGuard {
  UserAllocGuard() {
    // Nop
  }

  ~UserAllocGuard() {
    // Nop
  }
};

}  // namespace memory::shared

}  // namespace system

}  // namespace twist::rt::sim
