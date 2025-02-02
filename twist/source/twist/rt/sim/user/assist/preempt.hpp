#pragma once

#include "../scheduler/interrupt.hpp"
#include "../scheduler/preemption.hpp"

namespace twist::rt::sim {

namespace user::assist {

inline void PreemptionPoint(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  scheduler::Interrupt(call_site);
}

//

struct NoPreemptionGuard : scheduler::PreemptionGuard {
  NoPreemptionGuard()
      : scheduler::PreemptionGuard(false) {
    //
  }
};

}  // namespace user::assist

}  // namespace twist::rt::sim
