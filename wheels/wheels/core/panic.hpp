#pragma once

#include <wheels/core/source_location.hpp>
#include <wheels/core/string_builder.hpp>

namespace wheels {

[[noreturn]] void Panic(std::string_view error, SourceLocation where = SourceLocation::Current());

}  // namespace wheels

// Usage: WHEELS_PANIC("Internal error: " << e.what());

#define WHEELS_PANIC(error)                                                   \
  do {                                                                        \
    ::wheels::StringBuilder err_builder{};                                     \
    err_builder << error;                                                                           \
    ::wheels::Panic(err_builder.String(), WHEELS_HERE); \
  } while (false)
