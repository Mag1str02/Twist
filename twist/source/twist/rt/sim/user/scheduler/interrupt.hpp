#pragma once

#include <wheels/core/source_location.hpp>
#include <wheels/core/compiler.hpp>

namespace twist::rt::sim {

namespace user::scheduler {

void Interrupt(wheels::SourceLocation call_site = wheels::SourceLocation::Current());

}  // namespace scheduler

}  // namespace twist::rt::sim
