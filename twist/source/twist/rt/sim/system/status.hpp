#pragma once

#include <string_view>

namespace twist::rt::sim {

namespace system {

enum class Status : int {
  Ok = 0,
  LibraryAssert,
  UnhandledException,
  Deadlock,
  MemoryLeak,
  MemoryDoubleFree,
  InvalidMemoryAccess,
  DataRace,
  UserAbort,
  Pruned,
};

}  // namespace system

}  // namespace twist::rt::sim

#include <fmt/core.h>

namespace fmt {

template <>
struct formatter<twist::rt::sim::system::Status> {

  using Status = twist::rt::sim::system::Status;

  static std::string_view StatusToString(Status status) {
    switch (status) {
      case Status::Ok:
        return "Ok";
      case Status::LibraryAssert:
        return "Library assertion";
      case Status::UnhandledException:
        return "Unhandled exception";
      case Status::Deadlock:
        return "Deadlock";
      case Status::MemoryLeak:
        return "Memory leak";
      case Status::MemoryDoubleFree:
        return "Memory double-free";
      case Status::InvalidMemoryAccess:
        return "Invalid memory access";
      case Status::DataRace:
        return "Data race";
      case Status::UserAbort:
        return "User abort";
      case Status::Pruned:
        return "Pruned";
    }
  }

  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FmtContext>
  auto format(Status status, FmtContext& ctx) const {
    return ::fmt::format_to(ctx.out(), "{}", StatusToString(status));
  }
};

}  // namespace fmt
