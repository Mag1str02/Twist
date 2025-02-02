#pragma once

#include "sync_var.hpp"
#include "mutex_owner.hpp"
#include "mutex_traits.hpp"

#include <twist/trace/scope.hpp>
#include <twist/trace/attr/unit.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>

namespace twist::rt::sim {

namespace user::library::std_like {

// Tracing

namespace domain {

// Static domains
trace::Domain& Mutex();
trace::Domain& MutexImpl();

}  // namespace domain

//

class MutexImpl {
  enum State : uint32_t {
    Unlocked = 0,
    Locked = 1,
  };

 public:
  MutexImpl(wheels::SourceLocation source_loc)
      : locked_(source_loc) {
  }

  void Init(wheels::SourceLocation source_loc) {
    trace::Scope init{domain::MutexImpl()};

    locked_.Init(State::Unlocked, source_loc);
    contention_ = 0;
  }

  void Destroy() {
    trace::Scope destroy{domain::MutexImpl()};

    locked_.Destroy();
  }

  void Lock(wheels::SourceLocation call_site) {
    trace::Scope lock{domain::MutexImpl()};

    while (locked_.Exchange(State::Locked, std::memory_order::acquire, call_site) == State::Locked) {
      ++contention_;
      system::WaiterContext waiter{system::FutexType::MutexLock, "mutex::lock", call_site};
      syscall::FutexWait(locked_.FutexLoc(), State::Locked, &waiter);
      --contention_;
    }
  }

  bool TryLock(wheels::SourceLocation call_site) {
    trace::Scope try_lock{domain::MutexImpl()};

    uint32_t unlocked = State::Unlocked;
    return locked_.CompareExchangeWeak(unlocked, State::Locked, std::memory_order::acquire, std::memory_order::relaxed, call_site);
  }

  void Unlock(wheels::SourceLocation call_site) {
    trace::Scope unlock{domain::MutexImpl()};

    auto loc = locked_.FutexLoc();

    locked_.Store(State::Unlocked, std::memory_order::release, call_site);

    if (contention_ > 0) {
      system::WakerContext waker{"mutex::unlock", call_site};
      syscall::FutexWake(loc, 1, &waker);
    }
  }

 private:
  SyncVar locked_;
  size_t contention_;
};

class mutex {
 public:
  mutex(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(source_loc) {

    {
      trace::Scope ctor{domain::Mutex(), "mutex", source_loc};

      impl_.Init(source_loc);

      ctor.NoteInline("mutex::mutex");
    }
  }

  ~mutex() {
    trace::Scope dtor{domain::Mutex(), "~mutex"};

    impl_.Destroy();
  }

  // Non-copyable
  mutex(const mutex&) = delete;
  mutex& operator=(const mutex&) = delete;

  // Non-movable
  mutex(mutex&&) = delete;
  mutex& operator=(mutex&&) = delete;

  // std::mutex / Lockable

  void lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope lock{domain::Mutex(), "lock", call_site};

    lock.NoteInline("Enter lock");

    impl_.Lock(call_site);

    lock.NoteInline("Mutex acquired");

    owner_.Lock();
  }

  bool try_lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope try_lock{domain::Mutex(), "try_lock", call_site};

    bool acquired = impl_.TryLock(call_site);
    if (acquired) {
      owner_.Lock();
    }

    try_lock.NoteInline("try_lock() -> {}", acquired);

    return acquired;
  }

  void unlock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope unlock{domain::Mutex(), "unlock", call_site};

    owner_.Unlock();
    impl_.Unlock(call_site);

    unlock.NoteInline("Mutex released");
  }

 private:
  MutexImpl impl_;
  MutexOwner owner_;
};

template <>
struct MutexTraits<mutex> {
  static constexpr bool kNative = true;
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
