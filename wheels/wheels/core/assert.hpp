#pragma once

#include <wheels/core/compiler.hpp>
#include <wheels/core/panic.hpp>

/* Use-cases:
 * 1) Contract violations
 * 2) Unrecoverable errors
 */

////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

#define WHEELS_ASSERT(cond, error)                                   \
  do {                                                               \
    if (!(cond)) {                                                   \
      WHEELS_PANIC("Assertion '" << #cond << "' failed: " << error); \
    }                                                                \
  } while (false)

#else

#define WHEELS_ASSERT(cond, error) \
  do {                             \
    if (false) {                   \
      bool to_ignore = cond;       \
      WHEELS_UNUSED(to_ignore);    \
    }                              \
  } while (false)

#endif

////////////////////////////////////////////////////////////////////////////////

#define WHEELS_VERIFY(cond, error)                                   \
  do {                                                               \
    if (WHEELS_UNLIKELY(!(cond))) {                                  \
      WHEELS_PANIC("Assertion '" << #cond << "' failed: " << error); \
    }                                                                \
  } while (false)
