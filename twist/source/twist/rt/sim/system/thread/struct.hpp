#pragma once

#include <twist/rt/sim/system/fwd.hpp>
#include <twist/rt/sim/system/thread/user.hpp>
#include <twist/rt/sim/system/thread/id.hpp>
#include <twist/rt/sim/system/thread/state.hpp>
#include <twist/rt/sim/system/thread/attrs.hpp>
#include <twist/rt/sim/system/thread/entry.hpp>
#include <twist/rt/sim/system/futex/wait_queue.hpp>
#include <twist/rt/sim/system/futex/waiter.hpp>
#include <twist/rt/sim/system/futex/futex.hpp>
#include <twist/rt/sim/system/timer/timer.hpp>
#include <twist/rt/sim/system/call/status.hpp>
#include <twist/rt/sim/system/sync/atomic.hpp>
#include "twist/rt/sim/system/sync/thread.hpp"
#include <twist/rt/sim/system/memory.hpp>
#include "twist/rt/sim/system/scheduler/thread.hpp"

#include <twist/rt/sim/user/static/thread_local/storage.hpp>

#include <twist/rt/sim/user/fiber/fiber.hpp>

#include <sure/context.hpp>

#include <wheels/intrusive/list.hpp>

#include <optional>
#include <string_view>

namespace twist::rt::sim {

namespace system {

struct SchedulerTag {};
struct AliveTag {};

struct Thread final
    : private sure::ITrampoline,
      public wheels::IntrusiveListNode<Thread, SchedulerTag>,
      public wheels::IntrusiveListNode<Thread, AliveTag> {
 public:
  Thread() = default;

  void Reset(IThreadUserState* usr,
             Stack stk,
             ThreadId tid) {
    user = usr;
    stack = std::move(stk);
    id = tid;

    parent_id.reset();
    
    fiber = nullptr;

    state = ThreadState::Starting;
    main = false;

    attrs.preemptive = true;
    attrs.lock_free = false;

    futex = nullptr;
    timer = nullptr;
    waiter = nullptr;

    action = nullptr;
    load = 0;

    runs = 0;

    sync.Init();

    context.Setup(stack->MutView(), /*trampoline=*/this);
  }

  // sure::ITrampoline
  void Run() noexcept override {
    Entry(this);
  }

  IThreadUserState* user;  // Object from userspace
  std::optional<Stack> stack;
  sure::ExecutionContext context;
  ThreadState state;
  user::tls::Storage tls;
  ThreadId id;
  std::optional<ThreadId> parent_id;
  user::fiber::Fiber* fiber = nullptr;
  sure::StackView active_stack;
  bool main = false;

  ThreadAttrs attrs;

  sync::Action* action;
  uint64_t load;

  // For debugging
  const WaiterContext* waiter = nullptr;

  // Futex* system calls
  Futex* futex = nullptr;
  Timer* timer = nullptr;

  // For system calls
  call::Status status = call::Status::Ok;

  size_t runs{0};

  // For deadlock report from user-space
  std::string_view report;

  SchedulerThreadContext sched;
  sync::ThreadContext sync;
};

}  // namespace system

}  // namespace twist::rt::sim
