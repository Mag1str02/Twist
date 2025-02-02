#pragma once

#include "sync_var.hpp"
#include "mutex_owner.hpp"
#include "mutex_traits.hpp"

#include <twist/rt/sim/user/library/std_like/chrono.hpp>
#include <twist/rt/sim/user/library/std_like/chrono/deadline.hpp>

#include <twist/trace/scope.hpp>
#include <twist/trace/attr/unit.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>

namespace twist::rt::sim {

namespace user::library::std_like {

// Tracing

namespace domain {

// Static domains
trace::Domain& TimedMutex();
trace::Domain& TimedMutexImpl();

}  // namespace domain

//

class TimedMutexImpl {
  enum State : uint32_t {
    Unlocked = 0,
    Locked = 1,
  };

 public:
  explicit TimedMutexImpl(wheels::SourceLocation source_loc)
      : locked_(source_loc) {
  }

  void Init(wheels::SourceLocation source_loc) {
    trace::Scope init{domain::TimedMutexImpl()};

    locked_.Init(State::Unlocked, source_loc);
  }

  void Destroy() {
    trace::Scope destroy{domain::TimedMutexImpl()};

    locked_.Destroy();
  }

  // Non-copyable
  TimedMutexImpl(const TimedMutexImpl&) = delete;
  TimedMutexImpl& operator=(const TimedMutexImpl&) = delete;

  // Non-movable
  TimedMutexImpl(TimedMutexImpl&&) = delete;
  TimedMutexImpl& operator=(TimedMutexImpl&&) = delete;

  // std::mutex / Lockable

  void Lock(wheels::SourceLocation call_site) {
    trace::Scope lock{domain::TimedMutexImpl()};

    while (locked_.Exchange(State::Locked, std::memory_order::acquire, call_site) == State::Locked) {
      system::WaiterContext waiter{system::FutexType::MutexLock, "timed_mutex::lock", call_site};
      syscall::FutexWait(locked_.FutexLoc(), State::Locked, &waiter);
    }
  }

  bool TryLock(wheels::SourceLocation call_site) {
    trace::Scope try_lock{domain::TimedMutexImpl()};

    uint32_t unlocked = State::Unlocked;
    if (locked_.CompareExchangeWeak(unlocked, State::Locked, std::memory_order::acquire, std::memory_order::relaxed, call_site)) {
      return true;
    } else {
      return false;
    }
  }

  bool TryLockFor(chrono::Clock::duration timeout, wheels::SourceLocation call_site) {
    trace::Scope try_lock_for{domain::TimedMutexImpl()};

    system::WaiterContext waiter{system::FutexType::MutexTryLock, "timed_mutex::try_lock_for", call_site};
    return TryLockTimed(DeadLine::FromTimeout(timeout), &waiter);
  }

  bool TryLockUntil(chrono::Clock::time_point d, wheels::SourceLocation call_site) {
    trace::Scope try_lock_until{domain::TimedMutexImpl()};

    system::WaiterContext waiter{system::FutexType::MutexTryLock, "timed_mutex::try_lock_until", call_site};
    return TryLockTimed(DeadLine{d}, &waiter);
  }

  void Unlock(wheels::SourceLocation call_site) {
    trace::Scope unlock{domain::TimedMutexImpl()};

    auto futex = locked_.FutexLoc();
    locked_.Store(State::Unlocked, std::memory_order::release, call_site);
    system::WakerContext waker{"timed_mutex::unlock", call_site};
    syscall::FutexWake(futex, 1, &waker);
  }

 private:
  bool TryLockTimed(DeadLine d, system::WaiterContext* waiter) {
    while (locked_.Exchange(State::Locked, std::memory_order::acquire, waiter->source_loc) == State::Locked) {
      if (d.Expired()) {
        return false;
      }

      syscall::FutexWait(locked_.FutexLoc(), State::Locked, waiter);
    }
    return true;
  }

 private:
  SyncVar locked_;
};

class timed_mutex {
 public:
  timed_mutex(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(source_loc) {
    {
      trace::Scope ctor{domain::TimedMutex(), "timed_mutex"};

      impl_.Init(source_loc);

      ctor.Note("timed_mutex::timed_mutex", trace::NoteCtx{source_loc});
    }
  }

  ~timed_mutex() {
    trace::Scope dtor{domain::TimedMutex(), "~timed_mutex"};

    impl_.Destroy();
  }

  // Non-copyable
  timed_mutex(const timed_mutex&) = delete;
  timed_mutex& operator=(const timed_mutex&) = delete;

  // Non-movable
  timed_mutex(timed_mutex&&) = delete;
  timed_mutex& operator=(timed_mutex&&) = delete;

  // std::mutex / Lockable

  void lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope lock{domain::TimedMutex(), "lock", call_site};

    lock.NoteInline("Enter lock");

    impl_.Lock(call_site);
    owner_.Lock();

    lock.NoteInline("Mutex acquired");
  }

  bool try_lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope try_lock{domain::TimedMutex(), "unlock", call_site};

    bool acquired = impl_.TryLock(call_site);
    if (acquired) {
      owner_.Lock();
    }

    try_lock.NoteInline("try_lock() -> {}", acquired);

    return acquired;
  }

  bool try_lock_for(chrono::Clock::duration timeout, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope try_lock_for{domain::TimedMutex(), "try_lock_for", call_site};

    bool acquired = impl_.TryLockFor(timeout, call_site);
    if (acquired) {
      owner_.Lock();
    }

    try_lock_for.NoteInline("try_lock_for() -> {}", acquired);

    return acquired;
  }

  // NB: steady_clock and system_clock are both aliases of the same Clock,
  // so no template over clock type is required

  bool try_lock_until(chrono::Clock::time_point d, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope try_lock_until{domain::TimedMutex(), "try_lock_until", call_site};

    bool acquired = impl_.TryLockUntil(d, call_site);
    if (acquired) {
      owner_.Lock();
    }

    try_lock_until.NoteInline("try_lock_until() -> {}", acquired);

    return acquired;
  }

  void unlock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope unlock{domain::TimedMutex(), "unlock", call_site};

    unlock.NoteInline("Enter unlock");

    owner_.Unlock();
    impl_.Unlock(call_site);

    unlock.NoteInline("Mutex released");
  }

 private:
  TimedMutexImpl impl_;
  MutexOwner owner_;
};

template <>
struct MutexTraits<timed_mutex> {
  static constexpr bool kNative = true;
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
