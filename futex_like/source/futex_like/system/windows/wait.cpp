#include <futex_like/system/wait.hpp>

#include <synchapi.h>

namespace futex_like {

namespace system {

int WaitTimed(uint32_t* loc, uint32_t old, uint32_t millis) {
  WaitOnAddress(&loc, &old, sizeof(loc), millis);
  return 0;
}

int Wait(uint32_t* loc, uint32_t old) {
  WaitOnAddress(&loc, &old, sizeof(loc), UINT32_MAX);
  return 0;
}

int WakeOne(uint32_t* loc) {
  WakeByAddressSingle(loc);
  return 0;
}

int WakeAll(uint32_t* loc) {
  WakeByAddressAll(loc);
  return 0;
}

}  // namespace system

}  // namespace futex_like
