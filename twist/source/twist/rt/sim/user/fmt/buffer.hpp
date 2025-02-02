#pragma once

#include <fmt/core.h>

#include <wheels/memory/view.hpp>

#include <cstdlib>

namespace twist::rt::sim {

namespace user::fmt {

static inline constexpr size_t kBufferSize = 1024;

static wheels::MutableMemView StaticFormatBuffer() {
  static char buf[kBufferSize];
  return {buf, kBufferSize};
}

}  // namespace user::fmt

}  // namespace twist::rt::sim
