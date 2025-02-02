#pragma once

#include <wheels/core/source_location.hpp>

namespace twist::rt::thr {

namespace test {

void InjectFault(wheels::SourceLocation call_site = wheels::SourceLocation::Current());

}  // namespace test

}  // namespace twist::rt::thr
