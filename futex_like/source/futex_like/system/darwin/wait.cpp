#include <futex_like/system/wait.hpp>

extern "C" {
#include "ulock.h"
}

namespace futex_like {

namespace system {

uint32_t ToMicroSeconds(uint32_t millis) {
  return millis * 1000;
}

int WaitTimed(uint32_t* loc, uint32_t old, uint32_t millis) {
  return __ulock_wait(UL_COMPARE_AND_WAIT, loc, old, ToMicroSeconds(millis));
}

int Wait(uint32_t* loc, uint32_t old) {
  return WaitTimed(loc, old, /*millis=*/0);
}

int WakeOne(uint32_t* loc) {
  return __ulock_wake(UL_COMPARE_AND_WAIT, loc, 0);
}

int WakeAll(uint32_t* loc) {
  return __ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, loc, 0);
}

}  // namespace system

}  // namespace futex_like
