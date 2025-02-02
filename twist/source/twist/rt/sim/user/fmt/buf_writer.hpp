#pragma once

#include <cstdlib>

#include <wheels/memory/view.hpp>

#include <fmt/core.h>

namespace twist::rt::sim {

namespace user::fmt {

class BufWriter {
 public:
  BufWriter(wheels::MutableMemView buf)
      : start_(buf.Data()), buf_(buf.Data()), size_(buf.Size()) {
  }

  bool IsFull() const {
    return size_ == 0;
  }

  template <typename... Args>
  void Append(::fmt::format_string<Args...> format_str, Args&&... args) {
    auto written = ::fmt::format_to_n(buf_, size_, format_str, ::std::forward<Args>(args)...);

    buf_ += written.size;
    size_ -= written.size;
    written_ += written.size;
  }

  size_t Written() const {
    return written_;
  }

  std::string_view StringView() const {
    return {start_, Written()};
  }

 private:
  char* start_;
  char* buf_;
  size_t size_;
  size_t written_ = 0;
};

}  // namespace user::fmt

}  // namespace twist::rt::sim
