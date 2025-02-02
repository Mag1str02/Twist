#include "futex.hpp"

namespace twist::rt::sim {

namespace user::library::futex {

void WakeOne(WakeKey key, wheels::SourceLocation call_site) {
  system::WakerContext waker{"futex::WakeOne", call_site};
  syscall::FutexWake(key.loc, 1, &waker);
}

void WakeAll(WakeKey key, wheels::SourceLocation call_site) {
  system::WakerContext waker{"futex::WakeAll", call_site};
  syscall::FutexWake(key.loc, 0 /* 0 means all */, &waker);
}

}  // namespace user::library::futex

}  // namespace twist::rt::sim
