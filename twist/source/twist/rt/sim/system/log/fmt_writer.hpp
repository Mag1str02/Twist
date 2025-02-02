#pragma once

#include "fmt_buffer.hpp"

namespace twist::rt::sim {

namespace system {

namespace log {

class FmtWriter {
 public:
  FmtWriter(FmtBuffer* buf)
      : buf_(buf) {
  }

  template <typename... Args>
  void Append(::fmt::format_string<Args...> format_str, Args&&... args) {
    buf_->template Append(format_str, std::forward<Args>(args)...);
  }

 private:
  FmtBuffer* buf_;
};

}  // namespace log

}  // namespace system

}  // namespace twist::rt::sim
