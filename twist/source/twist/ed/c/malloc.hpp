#pragma once

/*
 * Drop-in replacement for libc {malloc,free}
 *
 * void* malloc(size_t size);
 * void free(void* ptr);
 *
 */

#include <twist/rt/cap/c/malloc.hpp>

namespace twist::ed {

namespace c {

using rt::cap::c::malloc;
using rt::cap::c::free;

}  // namespace c

}  // namespace twist::ed
