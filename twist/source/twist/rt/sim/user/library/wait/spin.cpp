#include "spin.hpp"

#include <twist/rt/sim/user/scheduler/model.hpp>
#include <twist/rt/sim/user/scheduler/spin_wait.hpp>

namespace twist::rt::sim {

namespace user {

bool SpinWait::ConsiderParking() const {
  if (user::scheduler::IsModelChecker()) {
    return true;
  } else {
    return yield_count_ > scheduler::SpinWaitYieldThreshold();
  }
}

}  // namespace user

}  // namespace twist::rt::sim
