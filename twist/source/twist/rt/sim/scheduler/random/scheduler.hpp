#pragma once

#include <twist/rt/sim/system/scheduler.hpp>

#include "params.hpp"
#include "thread_set.hpp"

#include <twist/wheels/random/wyrand.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/hash.hpp>

namespace twist::rt::sim {

namespace system::scheduler::random {

class Scheduler final : public IScheduler {
 public:
  using Params = random::Params;

 private:
  // Futex queue

  class WaitQueue : public IWaitQueue {
   public:
    WaitQueue(Scheduler* host)
        : host_(host) {
    }

    bool IsEmpty() const override {
      return set_.IsEmpty();
    }

    void Push(Thread* f) override {
      set_.Push(f);
    }

    Thread* Pop() override {
      return set_.PopRandom(host_->RandomNumber());
    }

    Thread* PopAll() override {
      return set_.PopAll();
    }

    bool Remove(Thread* f) override {
      return set_.Remove(f);
    }

   private:
    Scheduler* host_;
    ThreadSet set_;
  };

 public:
  Scheduler(Params params = Params())
      : params_(params),
        seed_(params_.seed),
        random_(seed_) {
  }

  bool NextSchedule() override {
    if (params_.max_runs && ++runs_ > *params_.max_runs) {
      return false;
    }

    wheels::HashCombine(seed_, runs_);  // Next seed
    return true;
  }

  void Start(Simulator* simulator) override {
    simulator_ = simulator;

    random_.Seed(seed_);

    preempt_count_ = 0;

    // lock-freedom
    lf_thread_count_ = 0;
    lf_progress_streak_ = 0;
    suspended_threads_.Reset();

    run_queue_.Reset();
    next_ = nullptr;

    WHEELS_VERIFY(run_queue_.IsEmpty(), "Inconsistent state");
  }

  void Interrupt(Thread* active) override {
    AfterStep(active);

    if (ToPreempt(active)) {
      Preempt(active);
    } else if (ToLfSuspend(active)) {
      LfSuspend(active);
    } else {
      next_ = active;  // Continue
    }
  }

  void SwitchTo(Thread* /*active*/, ThreadId /*next*/) override {
    WHEELS_PANIC("SwitchTo not supported by RandomScheduler");
  }

  void Yield(Thread* active) override {
    run_queue_.Push(active);
  }

  void Wake(Thread* waiter) override {
    run_queue_.Push(waiter);
  }

  void Spawn(Thread* thread) override {
    /*
     * Scheduler context:
     * f1 - time (in ticks) to preempt thread
     * f2 - 1
     */
    thread->sched.f1 = 0;
    thread->sched.f2 = 0;

    run_queue_.Push(thread);
  }

  void Exit(Thread* thread) override {
    (void)thread;  // Maybe unused
    assert(!thread->attrs.lock_free);
  }

  // Hints

  void LockFree(Thread* thread, bool flag) override {
    assert(thread->attrs.lock_free == flag);

    if (flag) {
      LfEnter(thread);
    } else {
      LfExit(thread);
    }
  }

  void Progress(Thread* thread) override {
    if (thread->attrs.lock_free) {
      LfProgress(thread);
    }
  }

  void NewIter() override {
    // No-op
  }

  // Run queue

  bool IsIdle() const override {
    return run_queue_.IsEmpty() && (next_ == nullptr);
  }

  Thread* PickNext() override {
    if (next_ != nullptr) {
      return std::exchange(next_, nullptr);
    }
    return PickNextFromRunQueue();
  }

  void Remove(Thread* thread) override {
    run_queue_.Remove(thread);
    suspended_threads_.Remove(thread);
  }

  // Futex

  IWaitQueuePtr NewWaitQueue() override {
    return std::make_unique<WaitQueue>(this);
  }

  // Random

  uint64_t RandomNumber() override {
    return random_.Next();
  }

  size_t RandomChoice(size_t alts) override {
    return random_.Next() % alts;
  }

  // Spurious

  bool SpuriousWakeup() override {
    return Spurious(params_.spurious_wakeups);
  }

  bool SpuriousTryFailure() override {
    return Spurious(params_.spurious_failures);
  }

  // Statistics

  size_t PreemptCount() const {
    return preempt_count_;
  }

  // Description

  std::string Name() const override {
    return "RandomScheduler";
  }

 private:
  Thread* PickNextFromRunQueue() {
    // Pick random thread
    Thread* next = run_queue_.PopRandom(random_.Next());
    // Set randomized time slice
    next->sched.f1 = simulator_->CurrTick() + RandomTimeSliceInTicks();
    return next;
  }

  size_t RandomTimeSliceInTicks() {
    return random_.Next() % params_.time_slice;
  }

  //

  void AfterStep(Thread* /*active*/) {
    //
  }

  // Preemption

  bool ToPreempt(Thread* active) {
    if (!active->attrs.preemptive) {
      return false;
    }

    return simulator_->CurrTick() >= active->sched.f1;
  }

  void Preempt(Thread* active) {
    run_queue_.Push(active);
    ++preempt_count_;
  }

  // Lock-freedom

  void LfEnter(Thread*) {
    ++lf_thread_count_;
  }

  void LfExit(Thread*) {
    assert(lf_thread_count_ > 0);
    --lf_thread_count_;

    LfResume();
  }

  static bool LfIsPotentialSuspensionPoint(sync::Action* next) {
    switch (next->type) {
      case sync::ActionType::AtomicRmwLoad:
      case sync::ActionType::AtomicStore:
        return true;
      default:
        return false;
    }
  }

  bool ToLfSuspend(Thread* active) {
    if (!params_.lock_free) {
      return false;
    }

    if (!active->attrs.lock_free || !active->attrs.preemptive) {
      return false;
    }

    if (active->sched.f2 == 1) {
      // Already suspended since last ReportProgress
      return false;
    }

    assert(active->action != nullptr);
    if (!LfIsPotentialSuspensionPoint(active->action)) {
      return false;
    }

    if (suspended_threads_.Size() >= params_.lf_suspended_threads_limit) {
      // Hit hard limit on number of suspended threads
      return false;
    }

    if (1 + suspended_threads_.Size() >= lf_thread_count_) {
      // At least one runnable lf thread required
      return false;
    }

    // TODO: Better
    // Arbitrary
    if (random_.Next() % 7 == 0) {
      return true;
    }

    return false;  // Continue
  }

  void LfSuspend(Thread* t) {
    t->state = ThreadState::LfSuspended;
    t->sched.f2 = 1;

    suspended_threads_.Push(t);
  }

  void LfProgress(Thread* t) {
    // Has been suspended since last Progress (~ method completion)
    t->sched.f2 = 0;

    ++lf_progress_streak_;

    if (lf_progress_streak_ >= params_.lf_progress_streak_to_resume) {
      // Progress streak is long enough
      LfResume();
      lf_progress_streak_ = 0;  // Reset
    }
  }

  void LfResume(Thread* t) {
    assert(t->attrs.lock_free);
    t->state = ThreadState::Runnable;
    run_queue_.Push(t);
  }

  void LfResume() {
    if (suspended_threads_.IsEmpty()) {
      return;
    }

    Thread* t = suspended_threads_.PopRandom(random_.Next());
    LfResume(t);
  }

  //

  bool Spurious(Rational p) {
    return (random_.Next() % p.denom) < p.num;
  }

 private:
  const Params params_;

  size_t seed_;
  twist::random::WyRand random_;
  size_t runs_ = 0;

  Simulator* simulator_;

  ThreadSet run_queue_;
  Thread* next_ = nullptr;

  // Lock-freedom
  size_t lf_thread_count_ = 0;
  size_t lf_progress_streak_ = 0;
  ThreadSet suspended_threads_;

  size_t preempt_count_ = 0;
};

}  // namespace system::scheduler::random

}  // namespace twist::rt::sim
