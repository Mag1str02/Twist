#pragma once

/*
 * Drop-in replacement for std::atomic<T>
 * https://en.cppreference.com/w/cpp/atomic/atomic
 *
 * Drop-in replacement for std::atomic_flag
 * https://en.cppreference.com/w/cpp/atomic/atomic_flag
 *
 * Contents:
 *   namespace twist::ed::std
 *     class atomic<T>
 *     class atomic_flag
 *     fun atomic_thread_fence
 */

#include <twist/rt/cap/std_like/atomic.hpp>
#include <twist/rt/cap/std_like/atomic_aliases.hpp>
#include <twist/rt/cap/std_like/atomic_flag.hpp>
#include <twist/rt/cap/std_like/atomic_thread_fence.hpp>

namespace twist::ed::std {

using rt::cap::std_like::atomic;
using rt::cap::std_like::atomic_flag;
using rt::cap::std_like::atomic_thread_fence;

using namespace rt::cap::std_like::atomic_aliases;  // NOLINT

}  // namespace twist::ed::std
