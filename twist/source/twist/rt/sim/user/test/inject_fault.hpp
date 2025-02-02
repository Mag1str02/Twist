#pragma once

#include <wheels/core/source_location.hpp>

namespace twist::rt::sim {

namespace user::test {

void InjectFault(wheels::SourceLocation call_site = wheels::SourceLocation::Current());

}  // namespace user::test

}  // namespace twist::rt::sim
