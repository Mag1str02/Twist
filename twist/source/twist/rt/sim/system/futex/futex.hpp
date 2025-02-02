#pragma once

#include "loc.hpp"
#include "wait_queue.hpp"

#include <cstdint>

namespace twist::rt::sim {

namespace system {

struct Futex {
  WaitQueue waiters;
};

}  // namespace system

}  // namespace twist::rt::sim
