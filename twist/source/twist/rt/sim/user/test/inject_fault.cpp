#include "inject_fault.hpp"

#include <twist/rt/sim/user/scheduler/interrupt.hpp>

namespace twist::rt::sim {

namespace user::test {

void InjectFault(wheels::SourceLocation call_site) {
  rt::sim::user::scheduler::Interrupt(call_site);
}

}  // namespace user::test

}  // namespace twist::rt::sim
