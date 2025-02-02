#pragma once

#include <fmt/core.h>

namespace twist::rt::thr {

namespace fmt {

template <typename ... Args>
void print(::fmt::format_string<Args...> format_str, Args&& ... args) {
  ::fmt::print(format_str, ::std::forward<Args>(args)...);
}

template <typename ... Args>
void println(::fmt::format_string<Args...> format_str, Args&& ... args) {
  ::fmt::println(format_str, ::std::forward<Args>(args)...);
}

}  // namespace fmt

}  // namespace twist::rt::thr
