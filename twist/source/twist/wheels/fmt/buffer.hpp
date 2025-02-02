#pragma once

#include <fmt/format.h>

namespace twist {

namespace fmt {

class AutoFmtBuffer {
 public:
  template <typename... Args>
  void Append(::fmt::format_string<Args...> format_str, Args&&... args) {
    ::fmt::format_to(std::back_inserter(buf_), format_str,
                   ::std::forward<Args>(args)...);
  }

  std::string_view StringView() const {
    return {buf_.data(), buf_.size()};
  }

 private:
  ::fmt::memory_buffer buf_;
};

}  // namespace fmt

}  // namespace twist
