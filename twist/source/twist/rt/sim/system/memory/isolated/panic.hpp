#pragma once

#include <wheels/core/assert.hpp>
#include <wheels/core/source_location.hpp>

// Soft panic
#include <twist/rt/sim/user/safety/assert.hpp>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

// Switch to system allocator and die ASAP
[[noreturn]] void HardPanic(std::string_view error, wheels::SourceLocation where);

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim

#define ___TWIST_ALLOCATOR_VERIFY(cond, error) \
  do {                                      \
    if (!(cond)) {                             \
      ::twist::rt::sim::system::memory::isolated::HardPanic(error, WHEELS_HERE); \
    }                                         \
  } while (false)
