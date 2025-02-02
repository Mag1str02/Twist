#include "interrupt.hpp"
#include "preemption.hpp"
#include "spurious.hpp"
#include "spin_wait.hpp"
#include "hardware_concurrency.hpp"

#include <twist/rt/sim/system/simulator.hpp>

#include <twist/rt/sim/user/library/std_like/atomic.hpp>
#include <twist/ed/static/var.hpp>

namespace twist::rt::sim {

namespace user::scheduler {

struct Contention {
  library::std_like::atomic<uint64_t> atom;

  Contention(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : atom(0, source_loc) {
  }
};

void Interrupt(wheels::SourceLocation call_site) {
  // TODO: better impl
  TWISTED_STATIC_VAR(Contention, interrupt);
  interrupt->atom.store(1, std::memory_order::relaxed, call_site);
}

bool SetThreadAttrPreemptive(bool target_status) {
  return system::Simulator::Current()->SysSetThreadAttrPreemptive(target_status);
}

PreemptionGuard::PreemptionGuard(bool target_status) {
  status_ = SetThreadAttrPreemptive(target_status);
}

PreemptionGuard::~PreemptionGuard() {
  SetThreadAttrPreemptive(status_);
}

bool SetThreadAttrLockFree(bool status) {
  return system::Simulator::Current()->SysSetThreadAttrLockFree(status);
}

void ReportProgress() {
  system::Simulator::Current()->SysSchedHintProgress();
}

bool SpuriousTryFailure() {
  return system::Simulator::Current()->SysSpuriousTryFailure();
}

bool IsModelChecker() {
  return system::Simulator::Current()->Scheduler()->IsModelChecker();
}

size_t SpinWaitYieldThreshold() {
  return system::Simulator::Current()->SpinWaitYieldThreshold();
}

size_t HardwareConcurrency() {
  return system::Simulator::Current()->HardwareConcurrency();
}

}  // namespace scheduler

}  // namespace twist::rt::sim
