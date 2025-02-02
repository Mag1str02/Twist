#pragma once

#include "fwd.hpp"

namespace twist::rt::sim {

namespace user::fiber {

struct FiberHandle {
  Fiber* fiber;

  FiberHandle()
      : fiber{nullptr} {
  }

  FiberHandle(Fiber* f)
      : fiber{f} {
  }
};

}  // namespace user::fiber

}  // namespace twist::rt::sim
