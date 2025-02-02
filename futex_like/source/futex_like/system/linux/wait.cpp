#include <futex_like/system/wait.hpp>

#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>
#include <linux/futex.h>

// There is no glibc wrapper for 'futex' syscall

static int futex(unsigned int* uaddr, int op, int val,
                 const struct timespec* timeout, int* uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
}

namespace futex_like {

namespace system {

static void SetTimeout(struct timespec& timeout, uint32_t millis) {
  timeout.tv_sec = millis / 1000;
  timeout.tv_nsec = (millis % 1000) * 1000'000;
}

int WaitTimed(uint32_t* loc, uint32_t old, uint32_t millis) {
  struct timespec timeout;
  SetTimeout(timeout, millis);

  return futex(loc, FUTEX_WAIT_PRIVATE, old, &timeout, nullptr, 0);
}

int Wait(uint32_t* loc, uint32_t old) {
  return futex(loc, FUTEX_WAIT_PRIVATE, old, nullptr, nullptr, 0);
}

int WakeOne(uint32_t* loc) {
  return futex(loc, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
}

int WakeAll(uint32_t* loc) {
  return futex(loc, FUTEX_WAKE_PRIVATE, INT32_MAX, nullptr, nullptr, 0);
}

}  // namespace system

}  // namespace futex_like
