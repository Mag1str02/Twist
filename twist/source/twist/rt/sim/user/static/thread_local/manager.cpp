#include "manager.hpp"

#include <twist/ed/static/var.hpp>

#include <twist/rt/sim/user/syscall/tls.hpp>

namespace twist::rt::sim {

namespace user {

namespace tls {

extern const uintptr_t kSlotUninitialized = 1;

TWISTED_STATIC_VAR(Manager, instance);

Manager& Manager::Instance() {
  return instance.Ref();
}

Storage& Manager::Tls() {
  return user::syscall::AccessTls();
}

}  // namespace tls

}  // namespace user

}  // namespace twist::rt::sim
