#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/memory.hpp>

namespace twist::rt::cap {

namespace assist {

using sim::user::assist::New;
using sim::user::assist::MemoryAccess;
using sim::user::assist::Access;
using sim::user::assist::Ptr;

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/assist/memory.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::thr::assist::New;
using rt::thr::assist::MemoryAccess;
using rt::thr::assist::Access;
using rt::thr::assist::Ptr;

}  // namespace assist

}  // namespace twist::rt::cap

#endif
