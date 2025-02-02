#pragma once

/*
 * Drop-in replacement for std::condition_variable
 * https://en.cppreference.com/w/cpp/thread/condition_variable
 *
 * Contents:
 *   namespace twist::ed::std
 *     class condition_variable
 */


#include <twist/rt/cap/std_like/condition_variable.hpp>

namespace twist::ed::std {

using rt::cap::std_like::condition_variable;

}  // namespace twist::ed::std
