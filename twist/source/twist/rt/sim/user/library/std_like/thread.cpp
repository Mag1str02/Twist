#include "thread.hpp"

// #include <wheels/core/assert.hpp>
#include <twist/rt/sim/user/safety/assert.hpp>

#include <twist/rt/sim/user/scheduler/hardware_concurrency.hpp>

#include <twist/trace/scope.hpp>

#include <fmt/core.h>

// std::exchange
#include <utility>

namespace twist::rt::sim {

namespace user::library::std_like {

// ThreadId

ThreadId::ThreadId()
    : fid(system::kImpossibleThreadId) {
}

std::ostream& operator<<(std::ostream& out, ThreadId id) {
  out << ::fmt::format("{0:x}", id.fid);
  return out;
}

// Synchronization

thread::StateBase::StateBase(wheels::SourceLocation source_loc)
    : ctor_source_loc(source_loc),
      sync{source_loc} {
  {
    trace::Scope init{domain::ThreadState()};
    sync.Init(2, source_loc);
  }
}

void thread::StateBase::DestroySelf() {
  {
    trace::Scope destroy{domain::ThreadState()};

    sync.Destroy();
  }

  delete this;
}


void thread::StateBase::Detach() {
  trace::Scope detach{domain::ThreadState()};

  syscall::Detach(id.fid);

  // User-space coordination between parent and child
  if (sync.FetchSub(1, std::memory_order::acq_rel) == 1) {
    // Last
    DestroySelf();  // Completed
  }
}

void thread::StateBase::Join(wheels::SourceLocation call_site) {
  trace::Scope join{domain::ThreadState()};

  system::WaiterContext waiter{system::FutexType::Thread, "thread::join", call_site};
  while (sync.Load(std::memory_order::acquire, call_site) == 2) {  // Not completed
    syscall::FutexWait(sync.FutexLoc(), 2, &waiter);
  }

  DestroySelf();
}

void thread::StateBase::AtThreadExit() noexcept {
  trace::Scope at_exit{domain::ThreadState()};

  if (sync.FetchSub(1, std::memory_order::acq_rel) == 1) {
    // Last ~ Detached
    DestroySelf();  // Detached
  } else {
    system::WakerContext waker{"thread exit", ctor_source_loc};
    syscall::FutexWake(sync.FutexLoc(), 1, &waker);
  }
}

// Thread

thread::thread()
    : state_(nullptr) {
}

thread::thread(StateBase* state, wheels::SourceLocation source_loc) {
  system::UserContext ctx{"thread::thread", source_loc};
  auto id = syscall::Spawn(state, &ctx);
  state->id = id;
  state_ = state;
}

thread::~thread() {
  ___TWIST_SIM_USER_VERIFY(!HasState(), "Thread join or detach required");
}

thread::thread(thread&& that)
   : state_(that.ReleaseState()) {
}

thread& thread::operator=(thread&& that) {
  ___TWIST_SIM_USER_VERIFY(!HasState(), "Cannot rewrite joinable thread object");
  state_ = that.ReleaseState();
  return *this;
}

ThreadId thread::get_id() const noexcept {
  if (HasState()) {
    return state_->id;
  } else {
    return ThreadId{};
  }
}

bool thread::joinable() const {
  return HasState();
}

void thread::detach(wheels::SourceLocation call_site) {
  ___TWIST_SIM_USER_VERIFY(HasState(), "Thread has already been detached");

  trace::Scope scope{domain::Thread(), "detach", call_site};

  ReleaseState()->Detach();

  scope.NoteInline("thread::detach()");
}

void thread::join(wheels::SourceLocation call_site) {
  ___TWIST_SIM_USER_VERIFY(HasState(), "Thread has been detached");

  trace::Scope join{domain::Thread(), "join", call_site};

  join.NoteInline("Enter thread::join()");

  ReleaseState()->Join(call_site);

  join.NoteInline("Thread joined");
}

unsigned int thread::hardware_concurrency() noexcept {
  return user::scheduler::HardwareConcurrency();
}

thread::native_handle_type thread::native_handle() {
  return state_;
}

void thread::swap(thread& that) {
  std::swap(state_, that.state_);
}

bool thread::HasState() const {
  return state_ != nullptr;
}

thread::StateBase* thread::ReleaseState() {
  return std::exchange(state_, nullptr);
}

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
