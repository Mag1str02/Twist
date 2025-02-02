#pragma once

#include "panic.hpp"

#if !defined(NDEBUG)

#define ___TWIST_SIM_USER_ASSERT(cond, error) \
  do {                              \
    if (!(cond)) {                  \
      twist::rt::sim::user::Panic(twist::rt::sim::system::SimStatus::LibraryAssert, "Assertion \"" #cond "\" failed: " #error); \
    }                               \
  } while (false)

#else

#define ___TWIST_SIM_USER_ASSERT(cond, error) \
  do {                              \
    /* No-op */                     \
  } while (false)

#endif


#define ___TWIST_SIM_USER_VERIFY(cond, error) \
  do {                              \
    if (!(cond)) {                  \
      twist::rt::sim::user::Panic(twist::rt::sim::system::Status::LibraryAssert, "Assertion \"" #cond "\" failed: " #error); \
    }                               \
  } while (false)
