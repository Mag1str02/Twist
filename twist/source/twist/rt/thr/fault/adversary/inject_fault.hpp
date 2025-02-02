#pragma once

#include <twist/rt/thr/fault/adversary/adversary.hpp>

namespace twist::rt::thr {
namespace fault {

inline void InjectFault() {
  Adversary()->Fault(FaultPlacement::Some);
}

inline void InjectFaultBefore() {
#if defined(__TWIST_INJECT_FAULT_BEFORE__)
  Adversary()->Fault(FaultPlacement::Before);
#endif
}

inline void InjectFaultAfter() {
#if defined(__TWIST_INJECT_FAULT_AFTER__)
  Adversary()->Fault(FaultPlacement::After);
#endif
}

}  // namespace fault
}  // namespace twist::rt::thr
