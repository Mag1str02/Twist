#pragma once

#include <wheels/core/source_location.hpp>

#include <fmt/core.h>

namespace fmt {

template <>
struct formatter<wheels::SourceLocation> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FmtContext>
  auto format(const wheels::SourceLocation& source_loc, FmtContext& ctx) const {
    return ::fmt::format_to(ctx.out(), "{}:{} [{}]", source_loc.File(), source_loc.Line(), source_loc.Function());
  }
};

}  // namespace fmt
