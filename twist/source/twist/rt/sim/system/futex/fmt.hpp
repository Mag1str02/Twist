#pragma once

#include "loc.hpp"

#include <wheels/core/compiler.hpp>
#include <wheels/core/panic.hpp>

#include <fmt/core.h>

namespace fmt {

template <>
struct formatter<twist::rt::sim::system::FutexLoc> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  using FutexLoc = twist::rt::sim::system::FutexLoc;
  using FutexAtomicType = twist::rt::sim::system::FutexAtomicType;
  using FutexAtomicPart = twist::rt::sim::system::FutexAtomicPart;

  template <typename FmtContext>
  auto FormatAtomicUint64Half(FutexLoc loc, FmtContext& ctx) const {
    if (loc.part == FutexAtomicPart::LowHalf) {
      return ::fmt::format_to(ctx.out(), "{:p}:low", loc.Atomic());
    } else if (loc.part == FutexAtomicPart::HighHalf) {
      return ::fmt::format_to(ctx.out(), "{:p}:high", loc.Atomic());
    } else {
      wheels::Panic("Unexpected futex location");
    }
  }

  template <typename FmtContext>
  auto format(FutexLoc loc, FmtContext& ctx) const {
    switch (loc.type) {
      case FutexAtomicType::Std:
      case FutexAtomicType::Uint32:
        return ::fmt::format_to(ctx.out(), "{:p}", loc.Atomic());
      case FutexAtomicType::Uint64Half:
        return FormatAtomicUint64Half(loc, ctx);
      default:
        WHEELS_UNREACHABLE();
    }
  }
};

}  // namespace fmt