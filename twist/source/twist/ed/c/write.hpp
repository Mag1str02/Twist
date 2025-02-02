#pragma once

/*
 * void write(int fd, const char* buf, size_t size);
 *
 * Write `size` bytes from `buf` to `fd`
 * - fd == 1 - stdout
 * - fd == 2 - stderr
 *
 */

#include <twist/rt/cap/c/write.hpp>

namespace twist::ed {

namespace c {

using rt::cap::c::write;

}  // namespace c

}  // namespace twist::ed
