#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/atomic_thread_fence.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::atomic_thread_fence;

}  // namespace twist::rt::cap::std_like

#else

#include <atomic>

namespace twist::rt::cap::std_like {

using ::std::atomic_thread_fence;

}  // namespace twist::rt::cap::std_like

#endif
