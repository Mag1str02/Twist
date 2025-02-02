#pragma once

#include "../sync/atomic.hpp"
#include "loc.hpp"

#include <wheels/core/assert.hpp>

#include <cstdint>

namespace twist::rt::sim {

namespace system {

uint64_t GetFutexValue(sync::AtomicVar* atomic, uint64_t atomic_value, FutexLoc loc);

}  // namespace system

}  // namespace twist::rt::sim
