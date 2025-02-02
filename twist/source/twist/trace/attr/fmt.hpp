#pragma once

#include "../attr.hpp"

#include "detail/str_buf.hpp"

#include <fmt/core.h>

namespace twist::trace {

namespace attr {

template <size_t BufSize = 64>
struct Fmt : AttrBase,
                 protected detail::StrBuf<BufSize> {
  const char* name;

  using FmtBuf = detail::StrBuf<BufSize>;

  template <typename ... Args>
  Fmt(const char* name, ::fmt::format_string<Args...> format_str, Args&&... args) {
    this->name = name;
    FmtBuf::FormatToBuf(format_str, ::std::forward<Args>(args)...);
  }

  const char* GetName() const noexcept override {
    return name;
  }

  AttrValue GetValue() const noexcept override {
    return AttrValue::Str(FmtBuf::BufStr());
  }
};

}  // namespace attr

}  // namespace twist::trace
