#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/std_like/condition_variable.hpp>

namespace twist::rt::cap::std_like {

using rt::sim::user::library::std_like::condition_variable;

}  // namespace twist::rt::cap::std_like

#elif defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/std_like/condvar.hpp>

namespace twist::rt::cap::std_like {

using condition_variable = rt::thr::fault::FaultyCondVar;  // NOLINT

}  // namespace twist::rt::cap::std_like

#else

#include <condition_variable>

namespace twist::rt::cap::std_like {

using ::std::condition_variable;

}  // namespace twist::rt::cap::std_like

#endif
