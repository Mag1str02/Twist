#include <twist/mod/thr.hpp>

#include <twist/rt/thr/fault/adversary/adversary.hpp>

#include <twist/rt/thr/logging/logging.hpp>

#include <wheels/core/panic.hpp>
#include <wheels/core/assert.hpp>

#include <fmt/core.h>

#if !defined(__TWIST_SIM__)

namespace twist::thr {

void Run(TestRoutine test) {
#if defined(__TWIST_FAULTY__)
  rt::thr::fault::Adversary()->Reset();
#endif

  test();

  rt::thr::log::FlushPendingLogMessages();

#if defined(__TWIST_FAULTY__)
  rt::thr::fault::Adversary()->PrintReport();
#endif
}

}  // namespace twist::thr

#else

namespace twist::thr {

void MultiThread(MainRoutine) {
  wheels::Panic("MultiThread is not supported in this build: TWIST_SIM=ON");
}

}  // namespace twist::thr

#endif
