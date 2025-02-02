#pragma once

/*
 * Drop-in replacement for std::shared_mutex
 * https://en.cppreference.com/w/cpp/thread/shared_mutex
 *
 * Contents:
 *   namespace twist::ed::std
 *     class shared_mutex
 */

#include <twist/rt/cap/std_like/shared_mutex.hpp>

#include "lock_guard.hpp"
#include "unique_lock.hpp"

namespace twist::ed::std {

using rt::cap::std_like::shared_mutex;

}  // namespace twist::ed::std
