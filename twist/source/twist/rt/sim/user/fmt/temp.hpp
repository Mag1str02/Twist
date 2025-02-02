#pragma once

#include "buffer.hpp"
#include "buf_writer.hpp"

#include <fmt/core.h>

#include <cstdlib>

namespace twist::rt::sim {

namespace user::fmt {

template <typename... Args>
std::string_view FormatTemp(::fmt::format_string<Args...> format_str, Args&&... args) {
  auto buf = StaticFormatBuffer();
  auto written = ::fmt::format_to_n(buf.Data(), buf.Size(), format_str, ::std::forward<Args>(args)...);
  return {buf.Data(), written.size};
}

template <typename... Args>
std::string_view FormatLnTemp(::fmt::format_string<Args...> format_str, Args&&... args) {
  auto buf = StaticFormatBuffer();

  char* data = buf.Data();
  size_t space_left = buf.Size();

  // Message
  auto message = ::fmt::format_to_n(data, space_left - 3, format_str, ::std::forward<Args>(args)...);
  size_t size = message.size;
  space_left -= message.size;

  // Line break
  auto line_break = ::fmt::format_to_n(data + message.size, space_left, "\n");
  size += line_break.size;

  return {buf.Data(), size};
}

inline BufWriter TempBufWriter() {
  return {StaticFormatBuffer()};
}

}  // namespace user::fmt

}  // namespace twist::rt::sim
