#pragma once

#include <fmt/format.h>

#include <wheels/memory/view.hpp>

namespace twist::trace {

namespace detail {

wheels::MutableMemView StaticFmtBuffer();

template <typename... Args>
std::string_view TmpFormat(::fmt::format_string<Args...> format_str,
                            Args&&... args) {
  auto buf = StaticFmtBuffer();
  auto written = ::fmt::format_to_n(buf.Data(), buf.Size(), format_str,
                                    ::std::forward<Args>(args)...);
  return {buf.Data(), written.size};
}

}  // namespace detail

}  // namespace twist::trace
