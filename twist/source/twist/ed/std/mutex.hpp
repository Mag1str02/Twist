#pragma once

/*
 * Drop-in replacement for std::{mutex, timed_mutex}
 *
 * https://en.cppreference.com/w/cpp/thread/mutex
 * https://en.cppreference.com/w/cpp/thread/timed_mutex
 *
 * Contents:
 *   namespace twist::ed::std
 *     class mutex
 *     class timed_mutex
 */

#include <twist/rt/cap/std_like/mutex.hpp>

#include "lock_guard.hpp"
#include "unique_lock.hpp"

namespace twist::ed::std {

using rt::cap::std_like::mutex;
using rt::cap::std_like::timed_mutex;

}  // namespace twist::ed::std
