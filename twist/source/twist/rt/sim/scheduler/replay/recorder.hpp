#pragma once

#include <twist/rt/sim/system/scheduler.hpp>

#include "params.hpp"
#include "schedule.hpp"

#include <twist/rt/sim/system/simulator.hpp>

#include <wheels/core/assert.hpp>

#include <fmt/core.h>

#include <map>

namespace twist::rt::sim {

namespace system::scheduler::replay {

class Recorder final : public IScheduler {
 public:
  using Params = replay::Params;
  using Schedule = replay::Schedule;

 private:
  // Futex queue

  class WaitQueue : public IWaitQueue {
   public:
    WaitQueue(Recorder* recorder, IWaitQueuePtr impl)
        : recorder_(recorder), impl_(std::move(impl)) {
    }

    bool IsEmpty() const override {
      return impl_->IsEmpty();
    }

    void Push(Thread* waiter) override {
      impl_->Push(waiter);
    }

    Thread* Pop() override {
      Thread* waiter = impl_->Pop();
      recorder_->RecordWakeOne(waiter);
      return waiter;
    }

    Thread* PopAll() override {
      Thread* waiter = impl_->PopAll();
      recorder_->RecordWakeAll(waiter);
      return waiter;
    }

    bool Remove(Thread* thread) override {
      return impl_->Remove(thread);
    }

   private:
    Recorder* recorder_;
    IWaitQueuePtr impl_;
  };

 public:
  Recorder(IScheduler* underlying)
      : underlying_(underlying) {
  }

  ~Recorder() {
  }

  bool NextSchedule() override {
    return false;
  }

  void Start(Simulator* simulator) override {
    underlying_->Start(simulator);
  }

  // System

  void Interrupt(Thread* active) override {
    underlying_->Interrupt(active);
  }

  void SwitchTo(Thread* active, ThreadId target) override {
    underlying_->SwitchTo(active, target);
  }

  void Yield(Thread* active) override {
    underlying_->Yield(active);
  }

  void Wake(Thread* waiter) override {
    underlying_->Wake(waiter);
  }

  void Spawn(Thread* thread) override {
    underlying_->Spawn(thread);
  }

  void Exit(Thread* thread) override {
    underlying_->Exit(thread);
  }

  // Run queue

  bool IsIdle() const override {
    return underlying_->IsIdle();
  }

  Thread* PickNext() override {
    Thread* next = underlying_->PickNext();
    RecordPickNext(next->id);
    return next;
  }

  void Remove(Thread* thread) override {
    underlying_->Remove(thread);
  }

  // Hints

  void LockFree(Thread* thread, bool flag) override {
    underlying_->LockFree(thread, flag);
  }

  void Progress(Thread* thread) override {
    underlying_->Progress(thread);
  }

  void NewIter() override {
    underlying_->NewIter();
  }

  // Futex


  IWaitQueuePtr NewWaitQueue() override {
    auto wq = underlying_->NewWaitQueue();
    return std::make_unique<WaitQueue>(this, std::move(wq));
  }

  // Random

  uint64_t RandomNumber() override {
    uint64_t v = underlying_->RandomNumber();
    RecordRandomNumber(v);
    return v;
  }

  size_t RandomChoice(size_t alts) override {
    size_t index = underlying_->RandomChoice(alts);
    RecordRandomChoice(index);
    return index;
  }

  // Spurious

  bool SpuriousWakeup() override {
    bool wake = underlying_->SpuriousWakeup();
    RecordSpuriousWakeup(wake);
    return wake;
  }

  bool SpuriousTryFailure() override {
    bool fail = underlying_->SpuriousTryFailure();
    RecordSpuriousTryFailure(fail);
    return fail;
  }

  //

  Schedule GetSchedule() const {
    return schedule_;
  }

  // Description

  std::string Name() const override {
    return fmt::format("Recorder({})", underlying_->Name());
  }

 private:
  void RecordPickNext(ThreadId next) {
    schedule_.push_back(decision::PickNext{next});
  }

  void RecordWakeOne(Thread* waiter) {
    if (waiter != nullptr) {
      schedule_.push_back(decision::WakeOne{waiter->id});
    } else {
      schedule_.push_back(decision::WakeOne{});
    }
  }

  void RecordWakeAll(Thread* waiter) {
    if (waiter != nullptr) {
      schedule_.push_back(decision::WakeAll{waiter->id});
    } else {
      schedule_.push_back(decision::WakeAll{});
    }
  }

  void RecordRandomNumber(uint64_t value) {
    schedule_.push_back(decision::RandomNumber{value});
  }

  void RecordRandomChoice(size_t index) {
    schedule_.push_back(decision::RandomChoice{index});
  }

  void RecordSpuriousWakeup(bool wake) {
    schedule_.push_back(decision::SpuriousWakeup{wake});
  }

  void RecordSpuriousTryFailure(bool fail) {
    schedule_.push_back(decision::SpuriousTryFailure{fail});
  }

 private:
  IScheduler* underlying_;
  Schedule schedule_;
};

}  // namespace system::scheduler::replay

}  // namespace twist::rt::sim
