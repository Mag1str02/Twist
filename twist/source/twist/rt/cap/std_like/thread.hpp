#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/thread.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::thread;

namespace this_thread = rt::sim::user::library::std_like::this_thread;

}  // namespace twist::rt::cap::std_like

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/std_like/thread.hpp>

namespace twist::rt::cap::std_like {

using thread = rt::thr::fault::FaultyThread;  // NOLINT

namespace this_thread = rt::thr::fault::this_thread;

}  // namespace twist::rt::cap::std_like

#else

#include <thread>

namespace twist::rt::cap::std_like {

using ::std::thread;

namespace this_thread = ::std::this_thread;

}  // namespace twist::rt::cap::std_like

#endif
