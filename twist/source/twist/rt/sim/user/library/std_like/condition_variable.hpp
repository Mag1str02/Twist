#pragma once

#include "sync_var.hpp"
#include "mutex.hpp"

#include "chrono/deadline.hpp"

#include <twist/trace/scope.hpp>
#include <twist/trace/attr/unit.hpp>

// std::unique_lock
#include <mutex>
// std::cv_status
#include <condition_variable>

namespace twist::rt::sim {

namespace user::library::std_like {

// Tracing

namespace domain {

// Static domains
trace::Domain& CondVar();
trace::Domain& CondVarImpl();

}  // namespace domain

//

class CondVarImpl {
 public:
  explicit CondVarImpl(wheels::SourceLocation source_loc)
      : count_(source_loc) {
  }

  void Init(wheels::SourceLocation source_loc) {
    trace::Scope init{domain::CondVarImpl()};

    count_.Init(0, source_loc);
  }

  void Destroy() {
    trace::Scope destroy{domain::CondVarImpl()};
    count_.Destroy();
  }

  template <typename LockType>
  void Wait(LockType& lock, wheels::SourceLocation call_site) {
    trace::Scope wait{domain::CondVarImpl()};

    std_like::mutex* mutex = lock.release();

    uint32_t old_count = count_.Load(std::memory_order::relaxed, call_site);
    mutex->unlock(call_site);

    {
      system::WaiterContext waiter{system::FutexType::CondVar, "condition_variable::wait", call_site};
      syscall::FutexWait(count_.FutexLoc(), old_count, &waiter);
    }

    mutex->lock(call_site);
    {
      LockType new_lock{*mutex, std::adopt_lock};
      lock.swap(new_lock);
    }
  }

  template <typename LockType>
  std::cv_status WaitTimed(LockType& lock, DeadLine d, wheels::SourceLocation call_site) {
    trace::Scope wait_timed{domain::CondVarImpl()};

    std_like::mutex* mutex = lock.release();

    uint32_t old_count = count_.Load(std::memory_order::relaxed, call_site);

    mutex->unlock(call_site);

    {
      system::WaiterContext waiter{system::FutexType::CondVar, "condition_variable::wait", call_site};
      syscall::FutexWaitTimed(count_.FutexLoc(), old_count, d.TimePoint(), &waiter);
    }

    mutex->lock(call_site);
    {
      LockType new_lock{*mutex, std::adopt_lock};
      lock.swap(new_lock);
    }

    // TODO: futex::Status for futex::WaitTimed
    if (d.Expired()) {
      return std::cv_status::timeout;
    } else {
      // Maybe spurious wakeup
      return std::cv_status::no_timeout;
    }
  }

  void NotifyOne(wheels::SourceLocation call_site) {
    trace::Scope notify_one{domain::CondVarImpl()};

    auto futex = count_.FutexLoc();

    count_.FetchAdd(1, std::memory_order::relaxed, call_site);

    system::WakerContext waker{"condition_variable::notify_one", call_site};
    syscall::FutexWake(futex, 1, &waker);
  }

  void NotifyAll(wheels::SourceLocation call_site) {
    trace::Scope notify_all{domain::CondVarImpl()};

    auto futex = count_.FutexLoc();

    count_.FetchAdd(1, std::memory_order::relaxed, call_site);

    system::WakerContext waker{"condition_variable::notify_all", call_site};
    syscall::FutexWake(futex, 0, &waker);
  }

 private:
  SyncVar count_;
};

class condition_variable {
 public:
  condition_variable(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(source_loc) {
    {
      trace::Scope ctor{domain::CondVar(), "condition_variable", source_loc};

      impl_.Init(source_loc);

      ctor.NoteInline("condition_variable::condition_variable");
    }
  }

  ~condition_variable() {
    trace::Scope dtor{domain::CondVar(), "~condition_variable"};

    impl_.Destroy();
  }

  // Non-copyable
  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  // Non-movable
  condition_variable(condition_variable&&) = delete;
  condition_variable& operator=(condition_variable&&) = delete;

  // std::condition_variable interface

  template <typename LockType>
  void wait(LockType& lock, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope wait{domain::CondVar(), "wait", call_site};

    wait.NoteInline("Enter wait");

    impl_.Wait(lock, call_site);

    wait.NoteInline("Exit wait");
  }

  template <typename LockType>
  std::cv_status wait_until(LockType& lock, chrono::Clock::time_point expiration_time, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope wait_until{domain::CondVar(), "wait_until", call_site};

    wait_until.NoteInline("Enter wait_until");

    auto status = impl_.WaitTimed(lock, DeadLine{expiration_time}, call_site);

    wait_until.NoteInline("Exit wait_until");

    return status;
  }

  template <typename LockType>
  std::cv_status wait_for(LockType& lock, chrono::Clock::duration timeout, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope wait_for{domain::CondVar(), "wait_for", call_site};

    wait_for.NoteInline("Enter wait_for");

    auto status = impl_.WaitTimed(lock, DeadLine::FromTimeout(timeout), call_site);

    wait_for.NoteInline("Exit wait_for");

    return status;
  }

  template <typename LockType, typename Predicate>
  void wait(LockType& lock, Predicate predicate, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    while (!predicate()) {
      impl_.Wait(lock, call_site);
    }
  }

  void notify_one(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope notify_one{domain::CondVar(), "notify_one", call_site};

    notify_one.NoteInline("Enter notify_one");

    impl_.NotifyOne(call_site);
  }

  void notify_all(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope notify_all{domain::CondVar(), "notify_all", call_site};

    notify_all.NoteInline("Enter notify_all");

    impl_.NotifyAll(call_site);
  }

 private:
  CondVarImpl impl_;
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
