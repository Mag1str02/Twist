#pragma once

#include "sync_var.hpp"
#include "chrono.hpp"

#include <twist/rt/sim/user/syscall/spawn.hpp>
#include <twist/rt/sim/user/syscall/futex.hpp>
#include <twist/rt/sim/user/syscall/yield.hpp>
#include <twist/rt/sim/user/syscall/sleep.hpp>
#include <twist/rt/sim/user/syscall/id.hpp>

#include <twist/trace/domain.hpp>
#include <twist/trace/attr/unit.hpp>

#include <wheels/core/source_location.hpp>

#include <fmt/core.h>

#include <ostream>
#include <tuple>

namespace twist::rt::sim {

namespace user::library::std_like {

// Tracing

namespace domain {

// Static domains
trace::Domain& Thread();
trace::Domain& ThreadState();

}  // namespace domain

// ThreadId

struct ThreadId {
  system::ThreadId fid;

  ThreadId();  // Invalid

  ThreadId(system::ThreadId id)
      : fid(id) {
  }

  bool operator==(const ThreadId that) const {
    return fid == that.fid;
  }

  bool operator!=(const ThreadId that) const {
    return fid != that.fid;
  }

  bool operator<(const ThreadId that) const {
    return fid < that.fid;
  }

  bool operator>(const ThreadId that) const {
    return fid > that.fid;
  }

  bool operator<=(const ThreadId that) const {
    return fid <= that.fid;
  }

  bool operator>=(const ThreadId that) const {
    return fid >= that.fid;
  }
};

std::ostream& operator<<(std::ostream& out, ThreadId);

}  // namespace user::library::std_like

}  // namespace twist::rt::sim

namespace fmt {

template <>
struct formatter<twist::rt::sim::user::library::std_like::ThreadId> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FmtContext>
  auto format(twist::rt::sim::user::library::std_like::ThreadId id, FmtContext& ctx) const {
    return ::fmt::format_to(ctx.out(), "{}", id.fid);
  }
};

}  // namespace fmt

namespace std {

template<>
struct hash<::twist::rt::sim::user::library::std_like::ThreadId> {
  std::size_t operator()(const twist::rt::sim::user::library::std_like::ThreadId id) const noexcept {
    return hash<twist::rt::sim::system::ThreadId>{}(id.fid);
  }
};

}  // namespace std


// Thread

namespace twist::rt::sim {

namespace user::library::std_like {

class thread {
 public:
  using id = ThreadId;

 private:
  struct StateBase : system::IThreadUserState {
    StateBase(wheels::SourceLocation source_loc);

    wheels::SourceLocation ctor_source_loc;
    SyncVar sync;  // Rendezvous state
    ThreadId id;

    wheels::SourceLocation SourceLoc() const noexcept override {
      return ctor_source_loc;
    }

    // Parent
    void Detach();
    void Join(wheels::SourceLocation call_site);

    // Child
    void AtThreadExit() noexcept override;

    void DestroySelf();
  };

  template <typename F>
  struct State : StateBase {
   public:
    State(wheels::SourceLocation source_loc, F&& f)
        : StateBase(source_loc), user_(std::move(f)) {
    }

    void RunUser() override {
      // Destroy user state at the end of RunUser
      auto user = std::move(user_);

      user(); // unhandled exception from user() -> Abort in Entry(Thread*)
    }

    F user_;
  };

  // NB: In userspace!
  template <typename F, typename ... Args>
  StateBase* AllocateState(wheels::SourceLocation source_loc, F&& fn, Args&& ... args) {
    auto closure = [fn = std::move(fn), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
      std::apply(fn, std::move(args));
    };
    return new State{source_loc, std::move(closure)};
  }

 public:
  using native_handle_type = StateBase*;

 public:
  thread();

  // https://www.cppstories.com/2021/non-terminal-variadic-args/

  template <typename F>
  thread(F fn, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : thread(AllocateState(source_loc, std::move(fn)), source_loc) {
  }

  template <typename F, typename A>
  thread(F fn, A&& a, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : thread(AllocateState(source_loc, std::move(fn), std::forward<A>(a)), source_loc) {
  }

  template <typename F, typename A, typename B>
  thread(F fn, A&& a, B&& b, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : thread(AllocateState(source_loc, std::move(fn), std::forward<A>(a),
                             std::forward<B>(b)), source_loc) {
  }

  template <typename F, typename A, typename B, typename C>
  thread(F fn, A&& a, B&& b, C&& c, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : thread(AllocateState(source_loc, std::move(fn), std::forward<A>(a),
                             std::forward<B>(b), std::forward<C>(c)), source_loc) {
  }

  template <typename F, typename A, typename B, typename C, typename D>
  thread(F fn, A&& a, B&& b, C&& c, D&& d, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : thread(AllocateState(source_loc, std::move(fn), std::forward<A>(a),
                             std::forward<B>(b), std::forward<C>(c),
                             std::forward<D>(d)),  source_loc) {
  }

  template <typename F, typename A, typename B, typename C, typename D, typename E>
  thread(F fn, A&& a, B&& b, C&& c, D&& d, E&& e, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : thread(AllocateState(source_loc, std::move(fn), std::forward<A>(a),
                             std::forward<B>(b), std::forward<C>(c),
                             std::forward<D>(d), std::forward<E>(e)),  source_loc) {
  }

  // More overloads?

  ~thread();

  // Non-copyable
  thread(const thread&) = delete;
  thread& operator=(const thread&) = delete;

  // Movable
  thread(thread&&);
  thread& operator=(thread&&);

  bool joinable() const;  // NOLINT
  void join(wheels::SourceLocation call_site = wheels::SourceLocation::Current());            // NOLINT
  void detach(wheels::SourceLocation call_site = wheels::SourceLocation::Current());          // NOLINT

  id get_id() const noexcept;  // NOLINT

  static unsigned int hardware_concurrency() noexcept;  // NOLINT

  native_handle_type native_handle();

  void swap(thread& that);

 private:
  thread(StateBase* state, wheels::SourceLocation source_loc);

  bool HasState() const;
  StateBase* ReleaseState();

 private:
  StateBase* state_;
};

// this_thread

namespace this_thread {

inline ThreadId get_id() {  // NOLINT
  return {syscall::GetId()};
}

inline void yield(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
  system::UserContext ctx{"std::this_thread::yield()", call_site};
  syscall::Yield(&ctx);
}

inline void sleep_for(std::chrono::nanoseconds delay) {  // NOLINT
  syscall::SleepFor(delay);
}

inline void sleep_until(chrono::Clock::time_point deadline) {  // NOLINT
  syscall::SleepUntil(deadline);
}

}  // namespace this_thread

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
