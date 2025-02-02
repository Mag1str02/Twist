#pragma once

/*
 * Drop-in replacement for ::fmt::print and ::fmt::println
 *
 * void fmt::print(format_str, args...)
 * void fmt::println(format_str, args...)
 *
 * https://fmt.dev/latest/index.html
 *
 * Usage:
 *
 * twist::cross::Run([] {
 *   twist::ed::fmt::println("Hello, {}!", "world");
 * });
 *
 */

#include <twist/rt/cap/fmt/print.hpp>

#include <fmt/core.h>

namespace twist::ed {

namespace fmt {

using rt::cap::fmt::print;
using rt::cap::fmt::println;

}  // namespace fmt

}  // namespace twist::ed
