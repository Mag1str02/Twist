#include "panic.hpp"

#include <twist/rt/sim/user/fmt/temp.hpp>
#include <twist/wheels/fmt/source_location.hpp>

#include <cstdio>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

static void WriteToStderr(std::string_view error, wheels::SourceLocation where) {
  // TODO: UB?
  // auto tmp = user::library::fmt::FormatLnTemp("Panicked at {}: {}", where, error);

  auto tmp = user::fmt::FormatLnTemp("Panicked at {}:{} [{}]: {}", where.File(), where.Line(), where.Function(), error);
  std::fwrite(tmp.data(), sizeof(char), tmp.size(), stderr);
}

void HardPanic(std::string_view error, wheels::SourceLocation where) {
  // NB: Avoid dynamic allocations
  WriteToStderr(error, where);
  std::abort();
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
