#include "memory.hpp"

#include <twist/rt/sim/user/scheduler/interrupt.hpp>

#include "../safety/panic.hpp"

#if defined(__TWIST_SIM_ISOLATION__)

#include <twist/rt/sim/system/simulator.hpp>

#include "../fmt/temp.hpp"

namespace twist::rt::sim {

namespace user::assist {

void MemoryAccess(void* addr, size_t size, wheels::SourceLocation call_site) {
  if (addr == nullptr) {
    user::Panic(system::Status::InvalidMemoryAccess, "Nullptr access", call_site);
  }

  scheduler::Interrupt(call_site);

  if (!system::Simulator::Current()->UserMemoryAccess(addr, size)) {
    user::Panic(system::Status::InvalidMemoryAccess,
        user::fmt::FormatTemp(
            "Cannot access heap at address = {}, size = {}", addr, size), call_site);
  }
}

}  // namespace user::assist

}  // namespace twist::rt::sim

#else

namespace twist::rt::sim {

namespace user::assist {

void MemoryAccess(void* ptr, size_t /*size*/, wheels::SourceLocation call_site) {
  if (ptr == nullptr) {
    user::Panic(system::Status::InvalidMemoryAccess, "Nullptr access", call_site);
  }

  scheduler::Interrupt();

  // TODO: read memory?
}

}  // namespace user::assist

}  // namespace twist::rt::sim

#endif