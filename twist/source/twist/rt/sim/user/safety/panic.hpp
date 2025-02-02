#pragma once

#include <twist/rt/sim/system/status.hpp>

#include <wheels/core/source_location.hpp>

#include <string_view>

namespace twist::rt::sim {

namespace user {

[[noreturn]] void Panic(
    system::Status status,
    std::string_view err,
    wheels::SourceLocation loc = wheels::SourceLocation::Current());

}  // namespace user

}  // namespace twist::rt::sim
