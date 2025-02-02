#pragma once

#include <cstddef>

#include <fmt/format.h>

namespace twist::trace {

namespace detail {

template <size_t Size>
class StrBuf {
 protected:
  template <typename ... Args>
  void FormatToBuf(::fmt::format_string<Args...> format_str, Args&&... args) {
    auto [_, size] = ::fmt::format_to_n(buf_, Size - 1, format_str, ::std::forward<Args>(args)...);
    buf_[size] = '\0';
  }

  const char* BufStr() const noexcept {
    return buf_;
  }

 private:
  char buf_[Size];
};

}  // namespace detail

}  // namespace twist::trace
