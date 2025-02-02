#pragma once

/*
 * Replacement for std::chrono::{system_clock, steady_clock}
 *
 * https://en.cppreference.com/w/cpp/chrono/system_clock
 * https://en.cppreference.com/w/cpp/chrono/steady_clock
 * https://en.cppreference.com/w/cpp/chrono/high_resolution_clock
 *
 * Contents:
 *   namespace twist::ed::std
 *     namespace chrono
 *     class system_clock
 *     class steady_clock
 *     class high_resolution_clock
 */

#include <twist/rt/cap/std_like/chrono.hpp>

namespace twist::ed::std {

namespace chrono {

using rt::cap::std_like::chrono::system_clock;
using rt::cap::std_like::chrono::steady_clock;
using rt::cap::std_like::chrono::high_resolution_clock;

}  // namespace chrono

}  // namespace twist::ed::std
