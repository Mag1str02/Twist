#pragma once

/*
 * Drop-in replacement for libc abort()
 * [[noreturn]] void abort();
 *
 * Aborts execution
 *
 * Usage:
 *
 * auto status = twist::cross::Run([] {
 *   twist::ed::c::abort();
 *
 *   WHEELS_UNREACHABLE();
 * });
 *
 */

#include <twist/rt/cap/c/abort.hpp>

namespace twist::ed {

namespace c {

// [[noreturn]]
using rt::cap::c::abort;

}  // namespace c

}  // namespace twist::ed
