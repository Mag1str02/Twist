#pragma once

#include <twist/rt/sim/system/scheduler.hpp>
#include <twist/rt/sim/system/simulator.hpp>

#include "params.hpp"
#include "schedule.hpp"

#include <wheels/core/assert.hpp>
#include <wheels/intrusive/list.hpp>

#include <fmt/core.h>

// std::exchange
#include <utility>

namespace twist::rt::sim {

namespace system::scheduler::dfs {

class Scheduler final : public IScheduler {
 public:
  using Params = dfs::Params;
  using Schedule = dfs::Schedule;

 private:
  class SortedList {
   public:
    bool IsEmpty() const {
      return size_ == 0;
    }

    size_t Size() const {
      return size_;
    }

    void Push(Thread* f) {
      size_t i = size_++;
      while (i > 0 && list_[i - 1]->id > f->id) {
        list_[i] = list_[i - 1];
        --i;
      }
      list_[i] = f;
    }

    Thread* Pop(size_t i) {
      Thread* f = list_[i];
      for (size_t j = i; j + 1 < size_; ++j) {
        list_[j] = list_[j + 1];
      }
      --size_;
      return f;
    }

    Thread* PopLast() {
      if (size_ == 0) {
        return nullptr;
      }
      return list_[--size_];
    }

    bool Remove(Thread* f) {
      for (size_t i = 0; i < size_; ++i) {
        if (list_[i]->id == f->id) {
          Pop(i);
          return true;
        }
      }
      return false;
    }

    void Clear() {
      size_ = 0;
    }

   private:
    std::array<Thread*, kMaxThreads> list_;
    size_t size_ = 0;
  };

 private:
  // Futex queue

  class FifoWaitQueue : public IWaitQueue {
   public:
    bool IsEmpty() const override {
      return queue_.IsEmpty();
    }

    void Push(Thread* waiter) override {
      return queue_.PushBack(waiter);
    }

    Thread* Pop() override {
      return queue_.PopFront();
    }

    Thread* PopAll() override {
      return Pop();
    }

    bool Remove(Thread* thread) override {
      if (thread->wheels::IntrusiveListNode<Thread, SchedulerTag>::IsLinked()) {
        queue_.Unlink(thread);
        return true;
      } else {
        return false;
      }
    }

   private:
    wheels::IntrusiveList<Thread, SchedulerTag> queue_;
  };

  class BranchingWaitQueue : public IWaitQueue {
   public:
    explicit BranchingWaitQueue(Scheduler* scheduler)
        : scheduler_(scheduler) {
    }

    bool IsEmpty() const override {
      return list_.IsEmpty();
    }

    void Push(Thread* waiter) override {
      return list_.Push(waiter);
    }

    Thread* Pop() override {
      return scheduler_->PickWaiter(list_);
    }

    Thread* PopAll() override {
      return list_.PopLast();  // TODO: inverse
    }

    bool Remove(Thread* thread) override {
      return list_.Remove(thread);
    }

   private:
    Scheduler* scheduler_;
    SortedList list_;
  };

 public:
  Scheduler(Params params = Params())
      : params_(params) {
  }

  bool NextSchedule() override {
    return Backtrack(schedule_);
  }

  void Start(Simulator* simulator) override {
    simulator_ = simulator;

    branch_index_ = 0;
    preempts_ = 0;
    steps_ = 0;

    run_queue_.Clear();
    active_ = nullptr;
  }

  // System

  void Interrupt(Thread* active) override {
    active_ = active;
  }

  void SwitchTo(Thread* /*active*/, ThreadId /*next*/) override {
    WHEELS_PANIC("SwitchTo not supported by DfsScheduler");
  }

  void Yield(Thread* active) override {
    run_queue_.Push(active);
  }

  void Wake(Thread* waiter) override {
    run_queue_.Push(waiter);
  }

  void Spawn(Thread* thread) override {
    run_queue_.Push(thread);
  }

  void Exit(Thread* thread) override {
    AddStep(thread);
  }

  // Hints

  void LockFree(Thread* /*thread*/, bool /*flag*/) override {
    // No-op
  }

  void Progress(Thread* /*thread*/) override {
    // No-op
  }

  void NewIter() override {
    // No-op
  }

  // Run queue

  bool IsIdle() const override {
    // (active_ != nullptr) => !run_queue_.IsEmpty()
    // WHEELS_ASSERT(!run_queue_.IsEmpty() || (active_ == nullptr), "Broken scheduler");
    return run_queue_.IsEmpty() && (active_ == nullptr);
  }

  Thread* PickNext() override {
    return PickNext(std::exchange(active_, nullptr));
  }

  void Remove(Thread* thread) override {
    run_queue_.Remove(thread);
  }

  // Futex

  IWaitQueuePtr NewWaitQueue() override {
    if (params_.wake_order) {
      return std::make_unique<BranchingWaitQueue>(this);
    } else {
      return std::make_unique<FifoWaitQueue>();
    }
  }

  // Random

  uint64_t RandomNumber() override {
    WHEELS_PANIC("RandomNumber not supported");
  }

  size_t RandomChoice(size_t alts) override {
    return Choose(alts, BranchType::RandomChoice);
  }

  // Spurious

  bool SpuriousWakeup() override {
    if (params_.spurious_wakeups) {
      return Either(BranchType::SpuriousWakeup);
    } else {
      return false;
    }
  }

  bool SpuriousTryFailure() override {
    if (params_.spurious_failures) {
      return Either(BranchType::SpuriousTryFailure);
    } else {
      return false;
    }
  }

  bool IsModelChecker() const override {
    return true;
  }

  // Description

  std::string Name() const override {
    return "DfsScheduler";
  }

 private:
  Thread* PickNext(Thread* active) {
    AddStep(active);

    Thread* preempted = nullptr;
    if (active) {
      if (Preempt(active)) {
        preempted = active;
      } else {
        return active;
      }
    }

    Thread* next = Pick(run_queue_, BranchType::PickNext);
    if (preempted != nullptr) {
      ++preempts_;
      run_queue_.Push(preempted);
    }
    ResetSteps(next);
    return next;
  }

  bool Preempt(Thread* active) {
    if (!active->attrs.preemptive) {
      return false;
    }

    if (run_queue_.IsEmpty()) {
      return false;  // Alone
    }

    if (ForcedPreempt(active)) {
      if (MaxPreemptsReached()) {
        Prune(::fmt::format(
            "Simulation blocked: force_preempt_after = {} blocked by max_preempts = {}",
            *params_.force_preempt_after_steps, *params_.max_preemptions));
        WHEELS_UNREACHABLE();
      } else {
        return true;  // Preempt
      }
    }

    if (MaxPreemptsReached()) {
      return false;  // Continue
    }

    // 0 - continue, 1 - preempt
    bool preempt = Either(BranchType::Preempt);

    if (preempt == 1) {
      return true;  // Preempt
    } else {
      return false;  // Continue
    }
  }

  void AddStep(Thread* running) {
    ++steps_;

    if (params_.max_steps && (steps_ > *params_.max_steps)) {
      Prune(::fmt::format("max_steps limit = {} reached", *params_.max_steps));
    }

    if (running) {
      running->sched.f1 += 1;
    }
  }

  void ResetSteps(Thread* next) {
    next->sched.f1 = 0;
  }

  bool ForcedPreempt(Thread* running) const {
    size_t steps = running->sched.f1;

    return params_.force_preempt_after_steps &&
           (steps >= *params_.force_preempt_after_steps);
  }

  bool MaxPreemptsReached() const {
      return params_.max_preemptions && (preempts_ >= *params_.max_preemptions);
  }

  Thread* PickWaiter(SortedList& wait_queue) {
    return Pick(wait_queue, BranchType::WakeOne);
  }

  Thread* Pick(SortedList& list, BranchType type) {
    if (list.IsEmpty()) {
      return nullptr;
    }
    if (list.Size() == 1) {
      return list.Pop(0);
    }

    size_t index = Choose(list.Size(), type);
    return list.Pop(index);
  }

  bool Either(BranchType type) {
    return Choose(2, type) == 1;
  }

  size_t Choose(size_t alts, BranchType type) {
    size_t branch = branch_index_++;
    if (branch < schedule_.size()) {
      // Replay
      WHEELS_ASSERT(schedule_[branch].type == type, "Unexpected branch type");
      return schedule_[branch].index;
    } else {
      schedule_.push_back(Branch{type, alts, 0});
      return 0;  // Go left
    }
  }

  void Prune(std::string why) {
    simulator_->Prune(std::move(why));
  }

 private:
  static bool Backtrack(Schedule& s) {
    while (!s.empty()) {
      Branch& last = s.back();
      if (last.index + 1 == last.alts) {
        s.pop_back();
      } else {
        last.index++;
        return true;
      }
    }
    return false;
  }

 private:
  const Params params_;

  Schedule schedule_{};
  size_t branch_index_ = 0;

  Simulator* simulator_;

  SortedList run_queue_;
  Thread* active_ = nullptr;

  size_t preempts_ = 0;
  size_t steps_ = 0;
};

}  // namespace system::scheduler::dfs

}  // namespace twist::rt::sim
