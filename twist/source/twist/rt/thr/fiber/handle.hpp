#pragma once

#include "fwd.hpp"

namespace twist::rt::thr {

namespace fiber {

struct FiberHandle {
  Fiber* fiber;

  FiberHandle()
    : fiber{nullptr} {
  }

  FiberHandle(Fiber* f)
    : fiber{f} {
  }
};

}  // namespace fiber

}  // namespace twist::rt::thr
