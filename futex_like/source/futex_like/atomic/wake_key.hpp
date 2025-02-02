#pragma once

#include <cstdint>

namespace futex_like {

namespace atomic {

struct WakeKey {
  uint32_t* loc;
};

}  // namespace atomic

}  // namespace futex_like
