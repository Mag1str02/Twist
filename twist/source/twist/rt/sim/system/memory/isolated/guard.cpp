#include "guard.hpp"

#include <twist/rt/sim/system/simulator.hpp>

#include <wheels/core/compiler.hpp>

#include <cstdlib>
#include <cstdio>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

bool UserAllocGuard::AllowUserAlloc(bool flag) {
#if defined(__TWIST_SIM_ISOLATION__)
  return system::Simulator::Current()->AllowUserAlloc(flag);
#else
  WHEELS_UNUSED(flag);
  return true;
#endif
}

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
