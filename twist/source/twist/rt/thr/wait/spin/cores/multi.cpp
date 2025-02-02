#include <twist/rt/thr/wait/spin/cores/multi.hpp>

#include <twist/rt/thr/wait/spin/relax.hpp>

namespace twist::rt::thr {

namespace cores::multi {

// https://github.com/crossbeam-rs/crossbeam/blob/master/crossbeam-utils/src/backoff.rs

static const size_t kSpinLimit = 6;
static const size_t kYieldLimit = 10;

void SpinWait::Spin() {
  if (count_ < kSpinLimit) {
    for (size_t j = 0; j < ((size_t)1 << count_); ++j) {
      CpuRelax();
    }
  } else {
    std::this_thread::yield();
  }

  ++count_;
}

bool SpinWait::ConsiderParking() const {
  return count_ > kYieldLimit;
}

}  // namespace cores::multi

}  // namespace twist::rt::thr
