#pragma once

/*
 * Drop-in replacement for std::random_device
 * https://en.cppreference.com/w/cpp/numeric/random/random_device
 *
 * Contents:
 *   namespace twist::ed::std
 *     class random_device
 */

#include <twist/rt/cap/std_like/random.hpp>

namespace twist::ed::std {

using rt::cap::std_like::random_device;

}  // namespace twist::ed::std
