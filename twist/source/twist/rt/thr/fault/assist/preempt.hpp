#pragma once

#include "../adversary/inject_fault.hpp"

namespace twist::rt::thr {

namespace fault::assist {

inline void PreemptionPoint() {
  InjectFault();
}

}  // namespace fault::assist

}  // namespace twist::rt::thr
