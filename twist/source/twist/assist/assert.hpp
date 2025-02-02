#pragma once

#include <twist/rt/cap/safety/panic.hpp>

namespace twist::assist {

using rt::cap::Panic;

}  // namespace twist::assist

#if defined(__TWIST_FAULTY__)

#define TWIST_ASSERT(cond, error) \
  do {                              \
    if (!(cond)) {                  \
      twist::assist::Panic("Assertion \"" #cond "\" failed: " #error); \
    }                               \
  } while (false)

#else

#define TWIST_ASSERT(cond, error) \
  do {                              \
    /* No-op */                     \
  } while (false)

#endif

#define TWIST_VERIFY(cond, error) \
  do {                              \
    if (!(cond)) {                  \
      twist::assist::Panic("Assertion \"" #cond "\" failed: " #error); \
    }                               \
  } while (false)
