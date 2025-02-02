#pragma once

#if defined(__TWIST_THR_SINGLE_CORE__)

#include <twist/rt/thr/wait/spin/cores/single.hpp>

namespace twist::rt::thr {

using cores::single::SpinWait;

}  // namespace twist::rt::thr

#else

#include <twist/rt/thr/wait/spin/cores/multi.hpp>

namespace twist::rt::thr {

using cores::multi::SpinWait;

}  // namespace twist::rt::thr

#endif

#include <twist/rt/thr/wait/spin/relax.hpp>

namespace twist::rt::thr {

using cores::CpuRelax;

}  // namespace twist::rt::thr
