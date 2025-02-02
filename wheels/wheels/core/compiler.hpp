#pragma once

#include <cstdlib>

// Suppress compiler warnings about unused variable/argument
#define WHEELS_UNUSED(x) \
  do {                   \
    (void)(x);           \
  } while (false)

// Unreachable by control flow
#define WHEELS_UNREACHABLE() std::abort()

// https://llvm.org/docs/BranchWeightMetadata.html#builtin-expect
#define WHEELS_LIKELY(cond) __builtin_expect(!!(cond), 1)
#define WHEELS_UNLIKELY(cond) __builtin_expect(!!(cond), 0)

// https://preshing.com/20120625/memory-ordering-at-compile-time/
#define WHEELS_COMPILER_BARRIER() asm volatile("" ::: "memory")
