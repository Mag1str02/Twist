#include "assist.hpp"

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user::fiber {

void ResetStack(StackView stack) {
  sim::system::Simulator::Current()->SysResetFiberStack(stack);
}

void NewFiber(Fiber* fiber) {
  fiber->stack = {};  // Unknown stack
  sim::system::Simulator::Current()->SysRegisterFiber(fiber);
}

void NewFiber(Fiber* fiber, StackView stack) {
  fiber->stack = stack;
  sim::system::Simulator::Current()->SysRegisterFiber(fiber);
}

Fiber* SwitchToFiber(Fiber* target) {
  return sim::system::Simulator::Current()->SysSwitchToFiber(target);
}

Fiber* CurrentFiber() {
  return sim::system::Simulator::Current()->SysCurrentFiber();
}

}  // namespace user::fiber

}  // namespace twist::rt::sim
