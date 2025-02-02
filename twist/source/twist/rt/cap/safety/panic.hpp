#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/safety/panic.hpp>

namespace twist::rt::cap {

[[noreturn]] inline void Panic(
    std::string_view err,
    wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  sim::user::Panic(sim::system::Status::UserAbort, err, loc);
}

}  // namespace twist::rt::cap

#else

#include <wheels/core/panic.hpp>

#include <twist/rt/thr/logging/logging.hpp>

namespace twist::rt::cap {

[[noreturn]] inline void Panic(
    std::string_view error,
    wheels::SourceLocation loc = wheels::SourceLocation::Current()) {
  thr::log::FlushPendingLogMessages();
  wheels::Panic(error, loc);
}

}  // namespace twist::rt::cap

#endif
