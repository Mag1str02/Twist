#pragma once

#include <twist/rt/cap/safety/panic.hpp>

namespace twist::assist {

using rt::cap::Panic;

}  // namespace twist::assist

#define TWIST_TEST_ASSERT(cond, error) \
  do {                              \
    if (!(cond)) {                  \
      twist::assist::Panic("Assertion \"" #cond "\" failed: " #error); \
    }                               \
  } while (false)
