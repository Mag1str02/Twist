#pragma once

namespace twist::rt::sim {

namespace user::scheduler {

// true - preemptive, false - non-preemptive
bool SetThreadAttrPreemptive(bool target_status);

struct PreemptionGuard {
  // Disable preemption
  PreemptionGuard(bool status = false);

  // Non-copyable
  PreemptionGuard(const PreemptionGuard&) = delete;
  PreemptionGuard& operator=(const PreemptionGuard&) = delete;

  // Non-movable
  PreemptionGuard(PreemptionGuard&&) = delete;
  PreemptionGuard& operator=(PreemptionGuard&&) = delete;

  // Enable preemption
  ~PreemptionGuard();

 private:
  bool status_;
};

}  // namespace user::scheduler

}  // namespace twist::rt::sim
