#include "wait_group.hpp"

#include <twist/rt/sim/user/syscall/futex.hpp>
#include <twist/rt/sim/user/scheduler/preemption.hpp>

#include <twist/trace/scope.hpp>

namespace twist::rt::sim {

namespace user::test {

WaitGroup::WaitGroup(wheels::SourceLocation ctor)
    : ctor_source_loc_(ctor) {
  wg_.SetVisibility(-128);
}

void WaitGroup::Join(wheels::SourceLocation call_site) {
  trace::Scope join{wg_, call_site};

  if (count_ == 0) {
    return;
  }

  {
    // NB: Happens-before any wg thread
    left_.store(count_);
  }

  // Spawn

  system::UserContext spawn_ctx{"test::WaitGroup", call_site};

  UserStateNode* s = head_;
  while (s != nullptr) {
    syscall::Spawn(s, &spawn_ctx);
    s = s->next;
  }

  do {
    system::WaiterContext waiter{system::FutexType::WaitGroup,
                                 "WaitGroup::Join", call_site};
    syscall::FutexWait(done_.FutexLoc(), 0, &waiter);
  } while (done_.load() != 1);
}

void WaitGroup::AtThreadExit() noexcept {
  trace::Scope at_thread_exit{wg_};

  scheduler::PreemptionGuard guard;  // <- Merge with the last user action

  // NB: Join release sequences of wg threads
  if (left_.fetch_sub(1) == 1) {
    done_.store(1);

    system::WakerContext waker{"WaitGroup completion", ctor_source_loc_};
    syscall::FutexWake(done_.FutexLoc(), 1, &waker);
  }
}

}  // namespace user::test

}  // namespace twist::rt::sim
