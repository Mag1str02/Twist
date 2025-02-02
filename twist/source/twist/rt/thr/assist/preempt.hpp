#pragma once

namespace twist::rt::thr::assist {

inline void PreemptionPoint() {
  // No-op
}

//

struct NoPreemptionGuard {
  NoPreemptionGuard() {
    // No-op
  }

  ~NoPreemptionGuard() {
    // No-op
  }
};

}  // namespace twist::rt::thr::assist
