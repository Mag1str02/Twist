#pragma once

/*
 * Drop-in replacement for std::thread
 * https://en.cppreference.com/w/cpp/thread/thread
 *
 * Contents:
 *   namespace twist::ed::std
 *     class thread
 *     namespace this_thread
 *       fun get_id
 *       fun yield
 *       fun sleep_for
 *       fun sleep_until
 */

#include <twist/rt/cap/std_like/thread.hpp>

namespace twist::ed::std {

using rt::cap::std_like::thread;

namespace this_thread {

using rt::cap::std_like::this_thread::get_id;
using rt::cap::std_like::this_thread::yield;
using rt::cap::std_like::this_thread::sleep_for;
using rt::cap::std_like::this_thread::sleep_until;

}  // namespace this_thread

}  // namespace twist::ed::std
