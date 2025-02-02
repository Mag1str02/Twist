#pragma once

#include <fmt/format.h>

#include "scope.hpp"

namespace fmt {

template <>
struct formatter<twist::trace::AttrValue> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  using ValueType = twist::trace::AttrValueType;

  template <typename FmtContext>
  auto format(const twist::trace::AttrValue& value, FmtContext& ctx) const {
    switch (value.ValueType()) {
      case ValueType::Uint:
        ::fmt::format_to(ctx.out(), "{}", value.GetUint());
        break;
      case ValueType::Ptr:
        ::fmt::format_to(ctx.out(), "{}", fmt::ptr(value.GetPtr()));
        break;
      case ValueType::Bool:
        ::fmt::format_to(ctx.out(), "{}", value.GetBool());
        break;
      case ValueType::Str:
        ::fmt::format_to(ctx.out(), "\"{}\"", value.GetStr());
        break;
      case ValueType::Unit:
        ::fmt::format_to(ctx.out(), "unit");
        break;
      default:
        WHEELS_UNREACHABLE();
    }

    return ctx.out();
  }
};

template <>
struct formatter<twist::trace::Scope> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FmtContext>
  static void PrintAttributes(FmtContext& ctx, twist::trace::AttrBase* attrs) {
    if (attrs == nullptr) {
      return;
    }

    ::fmt::format_to(ctx.out(), "[");
    size_t index = 0;
    auto* attr = attrs;
    while (attr != nullptr) {
      if (index > 0) {
        ::fmt::format_to(ctx.out(), ",");
      }
      ::fmt::format_to(ctx.out(), "{}={}", attr->GetName(), attr->GetValue());
      attr = attr->next;
      ++index;
    }
    ::fmt::format_to(ctx.out(), "]");
  }

  template <typename FmtContext>
  auto format(const twist::trace::Scope& scope, FmtContext& ctx) const {
    auto* domain = scope.GetDomain();

    // domain[attributes]::scope(id)[attributes]

    // Domain
    ::fmt::format_to(ctx.out(), "{}", domain->GetName());
    PrintAttributes(ctx, domain->GetAttrs());

    // Scope
    ::fmt::format_to(ctx.out(), "::{}(id={})", scope.GetName(), scope.GetId());
    PrintAttributes(ctx, scope.GetAttrs());
    return ctx.out();
  }
};

}  // namespace fmt
