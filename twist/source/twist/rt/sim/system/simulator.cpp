#include <twist/rt/sim/system/simulator.hpp>

#include <twist/rt/sim/user/static/manager.hpp>
#include <twist/rt/sim/system/futex/fmt.hpp>
#include <twist/rt/sim/system/futex/wake_count.hpp>
#include <twist/rt/sim/system/futex/get_value.hpp>
#include <twist/rt/sim/system/thread/count.hpp>
#include <twist/rt/sim/system/abort.hpp>

#include <twist/rt/sim/system/log/builder.hpp>

#include <twist/rt/sim/user/fmt/temp.hpp>

#include <twist/trace/runtime.hpp>

#include <twist/wheels/fmt/source_location.hpp>

#include <twist/build.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/panic.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include <limits>
#include <vector>
#include <utility>
#include <iostream>
#include <set>

namespace twist::rt::sim {

namespace system {

///

Simulator* Simulator::current = nullptr;

Simulator::Simulator(IScheduler* scheduler, Params params)
    : params_(params),
      scheduler_(scheduler),
      time_(ticker_, params),
      memory_(params),
      memory_model_() {
  ValidateParams();
}

// Operations invoked by running threads

void Simulator::SwitchTo(Thread* thread) {
  loop_context_.SwitchTo(thread->context);
}

call::Status Simulator::SystemCall(call::Handler handler) {
#ifndef NDEBUG
  if (debug_) {
    DebugSwitch(running_);
  }
#endif

  syscall_ = handler;
  running_->context.SwitchTo(loop_context_);
  return running_->status;

#ifndef NDEBUG
  if (debug_) {
    DebugResume(running_);
  }
#endif
}

// ~ System calls

void Simulator::SysNoop() {
  auto noop = [](Thread*) {
    return true;  // Resume;
  };

  SystemCall(noop);
}

void Simulator::LogSysSpawn(Thread* caller, const UserContext* ctx, ThreadId id) {
  log::EventBuilder builder;

  builder
      .Descr("Spawn thread #{} ({})", id, ctx->operation)
      .SetSourceLoc(ctx->source_loc)
      .SetScope(caller)
      .Done();

  SysLog(builder.EventView());
}

ThreadId Simulator::SysSpawn(IThreadUserState* user, const UserContext* ctx) {
  ThreadId id;

  auto spawn = [this, user, &id](Thread* me) {
    id = _SysSpawn(/*parent=*/me, user);
    return true;  // Continue;
  };

  SystemCall(spawn);

  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysSpawn(RunningThread(), ctx, id);
  }

  return id;
}

ThreadId Simulator::SysGetId() {
  WHEELS_ASSERT(AmIUser(), "Not a user");
  // NB: Without context switch
  return RunningThread()->id;
}

void Simulator::SysDetach(ThreadId id) {
  auto detach = [this, id](Thread*) {
    _SysDetach(id);
    return true;  // Continue
  };

  SystemCall(detach);
}

void Simulator::LogSysYield(Thread* thread, wheels::SourceLocation source_loc) {
  log::EventBuilder builder;

  builder
      .Descr("Yield")
      .SetSourceLoc(source_loc)
      .SetScope(thread)
      .Done();

  SysLog(builder.EventView());
}

void Simulator::SysYield(const UserContext* ctx) {
  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysYield(running_, ctx->source_loc);
  }

  auto yield = [this](Thread* me) {
    return _SysYield(me);
  };

  SystemCall(yield);
}

void Simulator::SysSwitchTo(ThreadId target) {
  auto switch_to = [this, target](Thread* me) {
    return _SysSwitchTo(me, target);
  };

  SystemCall(switch_to);
}

call::Status Simulator::SysSleepFor(Time::Duration delay) {
  return SysSleepUntil(time_.After(delay));
}

struct SleepTimer final : Timer {
  Thread* sleeper;

  void Alarm() noexcept override {
    Simulator::Current()->Wake(/*waker=*/nullptr, sleeper, WakeType::SleepTimer);
  }
};

call::Status Simulator::SysSleepUntil(Time::Instant deadline) {
  SleepTimer timer;
  timer.when = deadline;
  timer.sleeper = running_;

  auto sleep = [this, timer = &timer](Thread* me) -> bool {
    _SysSleep(me, timer);
    return false;  // Suspend
  };

  return SystemCall(sleep);
}

static bool IsUserAction(sync::Action* action) {
  return strstr(action->source_loc.File(), "twist/rt/sim") == nullptr;
}

static void PrintActionValue(log::FmtWriter output, sync::ValueType value_type, uint64_t value) {
  switch (value_type) {
    case sync::ValueType::Bool:
      output.Append("{}", (value == 1) ? "true" : "false");
      break;
    case sync::ValueType::Ptr:
      output.Append("{:p}", (void*)value);
      break;
    case sync::ValueType::Int:
      output.Append("{}", value);
      break;
    case sync::ValueType::Some:
      output.Append("{:#x}", value);
      break;
    case sync::ValueType::None:
      WHEELS_PANIC("Cannot print none value");
  }
}

static void SyncWriteLineComment(log::FmtWriter comment, Thread* thread, sync::Action* action) {
  if (sync::IsLoad(action->type)) {
    comment.Append(" -> ");
    PrintActionValue(comment, action->value_type, thread->load);
  } else if (sync::IsStore(action->type)) {
    comment.Append(" <- ");
    PrintActionValue(comment, action->value_type, action->value);
  }
}

static void DescrSync(log::FmtWriter descr, Thread* thread, sync::Action* action) {
  descr.Append("Sync action {}", action->operation);

  if (sync::IsRmwStep(action->type)) {
    if (action->type == sync::ActionType::AtomicRmwLoad) {
      descr.Append(" / [R]mw");
    } else {
      descr.Append(" / rm[W]");
    }
  } else if (action->type == sync::ActionType::AtomicThreadFence) {
    descr.Append(" / Fence");
  }

  descr.Append(" / {}", sync::MemoryOrderName(action->mo));

  descr.Append(" / addr = {:p}", action->loc);

  if (sync::IsLoad(action->type)) {
    descr.Append(" -> ");
    PrintActionValue(descr, action->value_type, thread->load);
  } else if (sync::IsStore(action->type)) {
    descr.Append(" <- ");
    PrintActionValue(descr, action->value_type, action->value);
  }
}

void Simulator::LogSysSync(Thread* thread, sync::Action* action) {
  log::EventBuilder builder;

  DescrSync(builder.DescrWriter(), thread, action);

  builder.SetSourceLoc(action->source_loc);
  SyncWriteLineComment(builder.LineCommentWriter(), thread, action);

  builder.SetScope(thread);

  builder.Done();

  SysLog(builder.EventView());
}

uint64_t Simulator::SysSync(sync::Action* action) {
  Thread* me = running_;

  auto sync = [this, action](Thread* me) {
    return _SysSync(me, action);
  };
  SystemCall(sync);

  if (IsSysLoggingEnabled() && IsUserAction(action)) {
    UserAllocGuard alloc_guard;
    LogSysSync(me, action);
  }

  return me->load;
}

void Simulator::SysAccessNonAtomicVar(sync::Access* access) {
  auto access_shared_var = [this, access](Thread* me) {
    _SysAccessNonAtomicVar(me, access);
    return true;
  };

  SystemCall(access_shared_var);
}

void Simulator::LogSysFutexWaitEnter(Thread* thread, FutexLoc loc, uint64_t old, const WaiterContext* ctx) {
  log::EventBuilder builder;

  builder
      .Descr("Entering FutexWait({}, {}) / user operation {}",
             loc, old, ctx->operation)
      .SetSourceLoc(ctx->source_loc)
      .SetComment("FutexWait({}, {})", loc, old)
      .SetScope(thread)
      .Done();

  SysLog(builder.EventView());
}

void Simulator::LogSysFutexWaitExit(Thread* thread, FutexLoc loc, uint64_t old, const WaiterContext* ctx) {
  log::EventBuilder builder;

  builder
      .Descr("Exiting FutexWait({}, {}) / user operation {}",
             loc, old, ctx->operation)
      .SetSourceLoc(ctx->source_loc)
      .SetComment("FutexWait({}, {})", loc, old)
      .SetScope(thread)
      .Done();

  SysLog(builder.EventView());
}

call::Status Simulator::SysFutexWait(FutexLoc loc, uint64_t old, const WaiterContext* ctx) {
  auto wait = [this, loc, old, ctx](Thread* me) {
    _SysFutexWait(me, loc, old, /*timer=*/nullptr, ctx);
    return false;  // Suspend
  };

  Thread* me = running_;

  // Before wait

  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysFutexWaitEnter(me, loc, old, ctx);
  }

  auto status = SystemCall(wait);

  if (WHEELS_UNLIKELY(me->state == ThreadState::Deadlocked)) {
    ReportFromDeadlockedThread(me);
    WHEELS_UNREACHABLE();
  }

  // After wait

  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysFutexWaitExit(me, loc, old, ctx);
  }

  return status;
}

struct FutexTimer : Timer {
  Thread* waiter = nullptr;

  void Alarm() noexcept override {
    Simulator::Current()->Wake(/*waker=*/nullptr, waiter, WakeType::FutexTimer);
  }
};

call::Status Simulator::SysFutexWaitTimed(FutexLoc loc, uint64_t old, Time::Instant d, const WaiterContext* ctx) {
  FutexTimer timer;
  timer.when = d;
  timer.waiter = running_;

  Thread* me = running_;

  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysFutexWaitEnter(me, loc, old, ctx);
  }

  auto wait_timed = [this, loc, old, timer = &timer, ctx](Thread* me) {
    _SysFutexWait(me, loc, old, timer, ctx);
    return false;  // Suspend
  };

  auto status = SystemCall(wait_timed);

  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysFutexWaitExit(me, loc, old, ctx);
  }

  return status;
}

static const char* FutexWakeCountToString(size_t count) {
  if (count == 1) {
    return "1";
  } else if (count == 0) {
    return "all";
  } else {
    WHEELS_PANIC("Unexpected FutexWake count");
  }
}

void Simulator::LogSysFutexWake(Thread* caller, FutexLoc loc, size_t count, const WakerContext* ctx) {
  log::EventBuilder builder;

  builder
      .Descr("FutexWake({}, {})", loc, FutexWakeCountToString(count))
      .SetSourceLoc(ctx->source_loc)
      .SetComment("FutexWake({}, {})", loc, FutexWakeCountToString(count))
      .SetScope(caller)
      .Done();

  SysLog(builder.EventView());
}

void Simulator::SysFutexWake(FutexLoc loc, size_t count, const WakerContext* ctx) {
  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysFutexWake(running_, loc, count, ctx);
  }

  auto wake = [this, loc, count](Thread* me) {
    _SysFutexWake(/*waker=*/me, loc, count);
    return true;  // Continue
  };

  SystemCall(wake);
}

void Simulator::SysMalloc(void* addr, size_t size) {
  auto malloc = [this, addr, size](Thread* me) {
    _SysMalloc(me, addr, size);
    return true;  // Resume
  };

  SystemCall(malloc);
}

void Simulator::SysFree(void* addr) {
  auto free = [this, addr](Thread* me) {
    _SysFree(me, addr);
    return true;  // Resume
  };

  SystemCall(free);
}

uint64_t Simulator::SysRandomNumber() {
  uint64_t number;

  auto random = [this, &number](Thread* /*me*/) {
    number = scheduler_->RandomNumber();
    return true;  // Resume
  };

  SystemCall(random);

  return number;
}

size_t Simulator::SysRandomChoice(size_t alts) {
  WHEELS_ASSERT(alts > 0, "alts == 0");

  if (alts == 1) {
    return 0;
  }

  size_t index;

  auto random = [this, alts, &index](Thread* /*me*/) {
    index = scheduler_->RandomChoice(alts);
    return true;  // Resume
  };

  SystemCall(random);

  return index;
}

bool Simulator::SysSpuriousTryFailure() {
  bool fail;

  auto failure = [this, &fail](Thread* /*me*/) {
    fail = scheduler_->SpuriousTryFailure();
    return true;  // Resume
  };

  SystemCall(failure);

  return fail;
}

void Simulator::LogSysWrite(int fd, std::string_view buf) {
  log::EventBuilder builder;

  builder
      .Descr("Write to fd {}: {}", fd, buf)
      .Done();

  SysLog(builder.EventView());
}

void Simulator::SysWrite(int fd, std::string_view buf) {
  auto write = [this, fd, buf](Thread*) {
    _SysWrite(fd, buf);
    return true;  // Resume
  };

  SystemCall(write);

  if (IsSysLoggingEnabled()) {
    UserAllocGuard alloc_guard;
    LogSysWrite(fd, buf);
  }
}

void Simulator::SysAbort(Status status) {
  auto abort = [this, status](Thread*) {
    _SysAbort(status);
    return false;  // Suspend
  };

  SystemCall(abort);  // Never returns

  WHEELS_UNREACHABLE();
}

bool Simulator::SysSetThreadAttrPreemptive(bool on) {
  return std::exchange(RunningThread()->attrs.preemptive, on);
}

bool Simulator::SysSetThreadAttrLockFree(bool on) {
  // Status before set
  bool prev;

  auto set_attr = [this, &prev, next = on](Thread* me) {
    prev = std::exchange(me->attrs.lock_free, next);
    scheduler_->LockFree(me, next);
    return true;  // Resume
  };

  SystemCall(set_attr);

  return prev;
}

void Simulator::SysSchedHintProgress() {
  auto hint = [this](Thread* me) {
    scheduler_->Progress(me);
    return true;  // Resume
  };

  SystemCall(hint);
}

void Simulator::SysSchedHintNewIter() {
  auto hint = [this](Thread*) {
    Checkpoint();
    scheduler_->NewIter();
    return true;  // Resume
  };

  SystemCall(hint);
}

void Simulator::SysThreadEnter(user::fiber::Fiber* fiber) {
#ifndef NDEBUG
  if (debug_) {
    DebugStart(running_);
  }
#endif

  auto enter = [this, fiber](Thread* me) {
    _SysThreadEnter(me, fiber);
    return true;  // Resume
  };

  SystemCall(enter);
}

void Simulator::SysThreadExit() {
  auto exit = [this](Thread* me) {
    _SysThreadExit(me);
    return false;  // Suspend
  };

  SystemCall(exit);  // Never returns

  WHEELS_UNREACHABLE();
}

static void MemsetStack(std::span<std::byte> stack, unsigned char chr) {
  std::memset(stack.data(), chr, stack.size());
}

void Simulator::SysResetFiberStack(std::span<std::byte> stack) {
  // TODO: Memory check

  if (params_.memset_stacks) {
    MemsetStack(stack, *params_.memset_stacks);
  }
}

uint64_t Simulator::SysRegisterFiber(user::fiber::Fiber* fiber) {
  auto register_fiber = [this, fiber](Thread* me) {
    _SysRegisterFiber(me, fiber);
    return true;  // Resume
  };

  SystemCall(register_fiber);

  return fiber->id;
}

void Simulator::LogSysSwitchToFiber(Thread* thread, user::fiber::Fiber* target_fiber) {
  log::EventBuilder builder;

  {
    auto writer = builder.DescrWriter();
    writer.Append("SwitchToFiber: ");

    if (thread->fiber->thread) {
      writer.Append("thread #{}", thread->id);
    } else {
      writer.Append("fiber #{}", thread->fiber->id);
    }

    writer.Append(" -> ");

    if (target_fiber->thread) {
      writer.Append("thread #{}", *(target_fiber->thread));
    } else {
      writer.Append("fiber #{}", target_fiber->id);
    }
  }

  builder.SetScope(thread);

  builder.Done();

  SysLog(builder.EventView());
}

user::fiber::Fiber* Simulator::SysSwitchToFiber(user::fiber::Fiber* target) {
  if (IsSysLoggingEnabled()) {
    LogSysSwitchToFiber(running_, target);
  }

  user::fiber::Fiber* from;
  auto switch_fiber = [this, &from, target](Thread* me) {
    from = _SysSwitchToFiber(me, target);
    return true;  // Resume
  };

  SystemCall(switch_fiber);

  return from;
}

user::fiber::Fiber* Simulator::SysCurrentFiber() {
  WHEELS_ASSERT(AmIUser(), "Not a user");
  WHEELS_ASSERT(running_->fiber != nullptr, "Invalid fiber");
  // Without context switch
  return running_->fiber;
}

void Simulator::SysLog(log::Event* event) {
  auto trace = [this, event](Thread* me) {
    _SysLog(me, event);
    return true;  // Resume
  };

  SystemCall(trace);
}

uint64_t Simulator::SysNewScopeId() {
  return ++next_scope_id_;
}

void Simulator::SysTrace(trace::Event* event) {
  log::Event log_event;

  log_event.descr = event->message;
  log_event.user = true;
  log_event.scope = event->scope;
  log_event.source.emplace();
  log_event.source->loc = event->loc;

  SysLog(&log_event);
}

// Scheduling

static size_t UnlimitedIterBudget() {
  return std::numeric_limits<size_t>::max();
}

Result Simulator::Run(MainRoutine main) {
  Init();
  StartLoop(std::move(main));
  RunLoop(UnlimitedIterBudget());
  StopLoop();

  return Result{
    .status=*status_,
    .failure_context=failure_context_,
    .std_out=stdout_.str(),
    .std_err=stderr_.str(),
    .threads=ThreadCount(),
    .iters=IterCount(),
    .digest=digest_.Digest()
  };
}

void Simulator::Start(MainRoutine main) {
  Init();
  StartLoop(std::move(main));
}

size_t Simulator::RunFor(size_t iters) {
  return RunLoop(iters);
}

bool Simulator::RunOneIter() {
  return RunLoop(1) == 1;
}

Result Simulator::Drain() {
  RunLoop(UnlimitedIterBudget());
  StopLoop();

  return Result{
      .status=*status_,
      .failure_context=failure_context_,
      .std_out=stdout_.str(),
      .std_err=stderr_.str(),
      .threads=ThreadCount(),
      .iters=IterCount(),
      .digest=digest_.Digest()
  };
}

Result Simulator::Burn() {
#if defined(__TWIST_SIM_ISOLATION__)
  {
    // Burn threads
    std::vector<Thread*> alive;
    for (Thread* f : alive_) {
      alive.push_back(f);
    }
    for (Thread* f : alive) {
      Burn(f);
    }
  }

  {
    // Burn static variables
    user::ss::Manager::Instance().BurnVars();
  }

  {
    // Reset internal data structures
    timer_queue_.Reset();
    memory_model_.Reset();
  }

  {
    // Burn memory
    memory_.Burn();
  }

  ResetCurrentScheduler();

  done_ = true;

  return Result{
      .status=*status_,
      .failure_context=failure_context_,
      .std_out=stdout_.str(),
      .std_err=stderr_.str(),
      .threads=ThreadCount(),
      .iters=IterCount(),
      .digest=digest_.Digest(),
  };

#else
  WHEELS_PANIC("Scheduler::Burn not supported: set TWIST_SIM_ISOLATION=ON");
#endif
}


void Simulator::ValidateParams() {
  // TODO
}

void Simulator::Init() {
  WHEELS_ASSERT(scheduler_ != nullptr, "Scheduler not set");

  memory_.Reset();
  allow_alloc_ = true;

  if (params_.min_stack_size) {
    memory_.SetMinStackSize(*params_.min_stack_size);
  }

  time_.Reset();

  ids_.Reset();

  digest_.Reset();
  // Cache decision
  digest_atomic_loads_ = DigestAtomicLoads();

  memory_model_.Reset();

  next_fiber_id_ = 0;
  next_scope_id_ = 0;

  timer_queue_.Reset();  // timer ids

  done_ = false;

  {
    // Simulator is one-shot
    std::stringstream{}.swap(stdout_);
    std::stringstream{}.swap(stderr_);
  }

  status_.reset();
  failure_context_.reset();

  step_index_ = 0;

  burnt_threads_ = 0;

  iter_ = 0;
  last_checkpoint_ = 0;

  futex_wait_count_ = 0;
  futex_wake_count_ = 0;
  yield_count_ = 0;
}

// IUserRoutine
void Simulator::RunUser() {
  (*main_)();
}

void Simulator::AtThreadExit() noexcept {
  // No-op
}

wheels::SourceLocation Simulator::SourceLoc() const noexcept {
  wheels::Panic("SourceLoc not supported for main thread");
}

void Simulator::SpawnMainThread(MainRoutine main) {
  main_ = std::move(main);

  Thread* main_thread = CreateThread(/*user=*/this);
  main_thread->main = true;
  scheduler_->Spawn(main_thread);
}

void Simulator::BurnDetachedThreads() {
  std::vector<Thread*> detached;

  for (Thread* f : alive_) {
    if (!f->main && !f->parent_id) {
      detached.push_back(f);
    }
  }

  for (Thread* f : detached) {
    Burn(f);
  }
}

void Simulator::MainCompleted() {
#if defined(__TWIST_SIM_ISOLATION__)
  if (params_.allow_detached_threads) {
    BurnDetachedThreads();
  }
#endif

  WHEELS_VERIFY(alive_.IsEmpty(), "There are alive threads after main completion");

  CheckMemory();

  status_ = Status::Ok;
}

void Simulator::StartLoop(MainRoutine main) {
  SetCurrentScheduler();
  scheduler_->Start(this);
  SpawnMainThread(std::move(main));
}

void Simulator::SetCurrentScheduler() {
  WHEELS_VERIFY(current == nullptr, "Cannot run simulator from another simulator");
  current = this;
}

void Simulator::ResetCurrentScheduler() {
  current = nullptr;
}

IRunQueue* Simulator::RunQueue() const {
  return scheduler_;
}

void Simulator::Tick() {
  ticker_.Tick();
}

void Simulator::Maintenance() {
  if (!timer_queue_.IsEmpty()) {
    PollTimers();
  }

  if (!IsDeadlock()) {
    SpuriousWakeups();
  }
}

// Precondition: run queue is empty
// true - keep running, false - stop run loop
bool Simulator::Idle() {
  CheckDeadlock();

  if (timer_queue_.IsEmpty()) {
    return false;  // Stop
  }

  if (PollTimers()) {
    return true;  // Keep running
  }

  time_.AdvanceTo(timer_queue_.NextDeadLine());
  PollTimers();
  return true;
}

void Simulator::Run(Thread* thread) {
  thread->state = ThreadState::Running;
  ++thread->runs;
  ++step_index_;
  running_ = thread;
  SwitchTo(thread);
  running_ = nullptr;
}

void Simulator::CompleteSync(Thread* thread) {
  sync::Action* action = std::exchange(thread->action, nullptr);
  if (action != nullptr) {
    thread->load = Sync(thread, action);
  }
}

void Simulator::LogPrevThreadStatus(ThreadId thread_id) {
  Thread* thread{nullptr};

  for (Thread* f : alive_) {
    if (f->id == thread_id) {
      thread = f;
      break;
    }
  }

  log::EventBuilder builder;

  if (thread == nullptr) {
    // Exit
    builder.Descr("Thread #{} terminated", thread_id);
  } else if (thread->waiter != nullptr) {
    // Futex
    builder
        .Descr("Thread #{} has been suspended ({})",
               thread_id, thread->waiter->operation)
        .SetSourceLoc(thread->waiter->source_loc)
        .SetScope(thread);
  } else if (thread->action != nullptr) {
    // Synchronization
    builder
        .Descr("Thread #{} has been preempted BEFORE {} operation",
               thread_id, thread->action->operation)
        .SetSourceLoc(thread->action->source_loc)
        .SetComment("Preempted")
        .SetScope(thread);
  } else {
    // Yield?
    builder.Descr("Thread #{} has yielded", thread_id);
  }

  builder.Done();

  if (thread != nullptr) {
    auto* scope = thread->fiber->scope;
    if ((scope != nullptr) && !scope->IsEventVisible()) {
      // return;  // Skip logging
    }
  }

  Log(builder.EventView());
}

void Simulator::LogIter(Thread* next) {
  if (prev_thread_id_ && next->id != *prev_thread_id_) {
    if (prev_thread_id_) {
      LogPrevThreadStatus(*prev_thread_id_);
    }

    {
      log::EventBuilder builder;
      builder
          .Descr("Switching to thread #{}", next->id)
          .Done();
      Log(builder.EventView());
    }

  } else if (!prev_thread_id_) {
    log::EventBuilder builder;
    builder
        .Descr("Running thread #{}", next->id)
        .Done();
    Log(builder.EventView());
  }
}

void Simulator::Iter(Thread* thread) {
  digest_.Iter(thread);

  auto thread_id = thread->id;

  if (IsSysLoggingEnabled()) {
    LogIter(thread);
  }

  CompleteSync(thread);

  do {
    Run(thread);
  } while (HandleSystemCall(thread));

  prev_thread_id_ = thread_id;
}

void Simulator::Iter() {
  ++iter_;

  Tick();

  if ((iter_ & 7) == 0) {
    Maintenance();
  }

  {
    Thread* next = RunQueue()->PickNext();
    Iter(next);
  }
}

struct SysAbortEx {
};

size_t Simulator::RunLoop(size_t iter_budget) {
  if (done_) {
    return 0;
  }

  size_t start_iter = iter_;

  try {
    do {
      while (!RunQueue()->IsIdle() && (iter_budget > 0)) {
        Iter();
        --iter_budget;
      }
    } while ((iter_budget > 0) && Idle());
  } catch (SysAbortEx) {
    Burn();
  }

  return (iter_ - start_iter);
}

bool Simulator::PollTimers() {
  bool progress = false;

  auto now = time_.Now();
  while (Timer* timer = timer_queue_.Poll(now)) {
    progress = true;
    timer->Alarm();
  }

  return progress;
}

void Simulator::AddTimer(Timer* timer) {
  timer_queue_.Add(timer);
}

void Simulator::CancelTimer(Timer* timer) {
  WHEELS_VERIFY(timer_queue_.Remove(timer), "Timer not found");
}

void Simulator::CheckStackPointer(Thread* thread) {
  std::byte* stack_ptr = (std::byte*)thread->context.StackPointer();

  auto stack = thread->active_stack;

  if (stack.size() == 0) {
    return;  // Unknown stack
  }

  std::byte* stack_start = stack.data();
  std::byte* stack_end = stack_start + stack.size();

  if ((stack_ptr < stack_start) || (stack_ptr >= stack_end)) {
    auto error = fmt::format(
        "User thread #{} stack pointer = {} is out of stack bounds: [{}, {})"
        "; possible reasons: stack overflow, invalid user-space fiber context switch, missing twist::assist::SwitchToFiber annotation",
        thread->id, fmt::ptr(stack_ptr), fmt::ptr(stack_start), fmt::ptr(stack_end));
    wheels::Panic(error);
  }
}

bool Simulator::HandleSystemCall(Thread* caller) {
  if constexpr (twist::build::IsolatedSim() && sure::build::StackPointerAvailable()) {
    if (params_.check_stack_pointer) {
      CheckStackPointer(caller);
    }
  }

  if (bool resume = syscall_(caller); resume) {
    caller->status = call::Status::Ok;
    return true;  // Resume caller
  } else {
    return false;  // Suspend caller
  }
}

ThreadId Simulator::_SysSpawn(Thread* parent, IThreadUserState* user) {
  Thread* child = CreateThread(user);
  child->parent_id = parent->id;
  memory_model_.Spawn(parent, child);
  scheduler_->Spawn(child);
  return child->id;
}

void Simulator::_SysDetach(ThreadId id) {
  for (Thread* f : alive_) {
    if (f->id == id) {
      f->parent_id.reset();
    }
  }
}

bool Simulator::_SysYield(Thread* thread) {
  // WHEELS_VERIFY(thread->preemptive, "Yield is not supported in non-preemptive mode");

  ++yield_count_;  // Statistics
  scheduler_->Yield(thread);
  return false;  // Always reschedule
}

bool Simulator::_SysSwitchTo(Thread* active, ThreadId next) {
  WHEELS_VERIFY(active->attrs.preemptive, "SwitchTo is not supported in non-preemptive mode");
  // TODO: active->id == next?
  scheduler_->SwitchTo(active, next);
  return false;  // Always reschedule
}

void Simulator::_SysSleep(Thread* sleeper, Timer* timer) {
  AddTimer(timer);

  {
    sleeper->state = ThreadState::Sleeping;

    sleeper->futex = nullptr;
    sleeper->timer = timer;
    sleeper->waiter = nullptr;
  }
}


bool Simulator::_SysSync(Thread* thread, sync::Action* action) {
  thread->action = action;
  scheduler_->Interrupt(thread);
  return false;
}

void Simulator::_SysAccessNonAtomicVar(Thread* thread, sync::Access* access) {
  auto race = memory_model_.AccessNonAtomicVar(thread, access);

  if (race) {
    auto type = [](sync::AccessType a) {
      return sync::IsRead(a) ? "READ" : "WRITE";
    };

    stderr_ << "Data race detected on memory location " << access->loc << ": " << std::endl;
    stderr_ << fmt::format("- Current {} from thread #{} at {}", type(access->type), thread->id, access->source_loc) << std::endl;
    stderr_ << fmt::format("- Previous {} from thread #{} at {}", type(race->type), race->thread, race->source_loc) << std::endl;

    _SysAbort(Status::DataRace);
  }
}

static bool VulnerableToSpuriousWakeup(const WaiterContext* waiter) {
  switch (waiter->type) {
    case FutexType::Futex:
    case FutexType::Atomic:
    case FutexType::CondVar:
    case FutexType::MutexTryLock:
      return true;
    default:
      return false;
  }
}

Futex* Simulator::GetFutex(FutexLoc loc) {
  return &futex_[loc.FutexKey()];
}

void Simulator::_SysFutexWait(Thread* waiter, FutexLoc loc, uint64_t old, Timer* timer, const WaiterContext* ctx) {
  // WHEELS_VERIFY(waiter->preemptive, "FutexWait is not supported in non-preemptive mode");

  // Statistics
  ++futex_wait_count_;

  if (VulnerableToSpuriousWakeup(ctx)) {
    if (scheduler_->SpuriousWakeup()) {
      ScheduleWaiter(waiter, call::Status::Interrupted);
      return;
    }
  }

  sync::AtomicVar* atomic = memory_model_.FutexAtomic(loc.Atomic());

  WHEELS_VERIFY(atomic != nullptr, "Atomic not found in SysFutexWait");

  uint64_t atomic_value = memory_model_.FutexLoad(waiter, atomic);

  uint64_t futex_value = GetFutexValue(atomic, atomic_value, loc);

  if (futex_value != old) {
    ScheduleWaiter(waiter, call::Status::Ok);
    return;
  }

  auto futex = GetFutex(loc);

  futex->waiters.Push(waiter);

  if (timer != nullptr) {
    AddTimer(timer);
  }

  {
    waiter->state = ThreadState::Parked;

    waiter->futex = futex;
    waiter->timer = timer;
    waiter->waiter = ctx;
  }
}

void Simulator::_SysFutexWake(Thread* waker, FutexLoc loc, size_t count) {
  Futex* futex = GetFutex(loc);

  if (count == 1) {
    // One
    if (Thread* waiter = futex->waiters.Pop(); waiter != nullptr) {
      Wake(waker, waiter, WakeType::FutexWake);
    }
  } else if (count == FutexWakeCount::All) {
    // All
    while (Thread* waiter = futex->waiters.PopAll()) {
      Wake(waker, waiter, WakeType::FutexWake);
    }
  } else {
    WHEELS_PANIC("'count' value " << count << " not supported");
  }

  // Statistics
  ++futex_wake_count_;
}

void Simulator::_SysMalloc(Thread* thread, void* addr, size_t size) {
  if (params_.malloc_happens_before) {
    memory_model_.Malloc(thread, addr, size);
  }
}

void Simulator::_SysFree(Thread* thread, void* addr) {
  if (params_.malloc_happens_before) {
    memory_model_.Free(thread, addr);
  }
}

void Simulator::_SysWrite(int fd, std::string_view buf) {
  if (fd == 1) {
    stdout_.write(buf.data(), buf.size());
    if (params_.forward_stdout && !silent_) {
      std::cout.write(buf.data(), buf.size());
    }
  } else if (fd == 2) {
    stderr_.write(buf.data(), buf.size());
  } else {
    // TODO: Abort simulation
    WHEELS_PANIC("Cannot write to fd " << fd);
  }
}

void Simulator::_SysAbort(Status status) {
#if defined(__TWIST_SIM_ISOLATION__)
  status_ = status;
  throw SysAbortEx{};
#else
  WHEELS_UNUSED(status);  // TODO
  wheels::Panic(stderr_.str());
#endif
}

void Simulator::_SysThreadEnter(Thread* thread, user::fiber::Fiber* fiber) {
  fiber->thread = thread->id;
  fiber->id = NextFiberId();
  fiber->stack = thread->stack->MutView();

  thread->fiber = fiber;
  thread->active_stack = thread->stack->MutView();
}

void Simulator::_SysThreadExit(Thread* thread) {
  scheduler_->Exit(thread);

  bool main = thread->main;

  DestroyThread(thread);

  if (main) {
    MainCompleted();
  }
}

user::fiber::Fiber* Simulator::_SysSwitchToFiber(Thread* thread, user::fiber::Fiber* target_fiber) {
  if (thread->fiber->id == target_fiber->id) {
    auto error = fmt::format("Invalid assist::SwitchToFiber: target fiber coincides with the current fiber");
    wheels::Panic(error);
  }

  thread->active_stack = target_fiber->stack;
  return std::exchange(thread->fiber, target_fiber);
}

uint64_t Simulator::_SysRegisterFiber(Thread*, user::fiber::Fiber* fiber) {
  // TODO: Do not write directly to user-space object
  fiber->id = NextFiberId();
  fiber->thread = std::nullopt;
  return fiber->id;
}

void Simulator::_SysLog(Thread* me, log::Event* event) {
  // Fill scheduling context
  event->sched.emplace();
  event->sched->thread_id = me->id;
  if (!me->fiber->thread) {
    event->sched->fiber_id = me->fiber->id;
  }
  event->sched->iter = iter_;

  Log(event);
}

static bool IsDigestableLoad(sync::ActionType action) {
  switch (action) {
    case sync::ActionType::AtomicLoad:
    case sync::ActionType::AtomicRmwLoad:
      return true;
    default:
      return false;
  }
}

void Simulator::DigestAtomicLoad(uint64_t repr) {
  if (digest_atomic_loads_) {
    digest_.Load(repr);
  }
}

uint64_t Simulator::Sync(Thread* thread, sync::Action* action) {
  uint64_t r = memory_model_.Sync(thread, action);

  if (IsDigestableLoad(action->type)) {
    DigestAtomicLoad(r);
  }

  return r;
}

void Simulator::ScheduleWaiter(Thread* waiter, call::Status status) {
  waiter->status = status;
  scheduler_->Wake(waiter);
}

static const char* WakeTypeToString(WakeType w) {
  switch (w) {
    case WakeType::FutexWake:
      return "futex::Wake";
    case WakeType::FutexTimer:
      return "futex::WaitTimed";
    case WakeType::Spurious:
      return "spurious wakeup";
    case WakeType::SleepTimer:
      return "sleep_for";
    default:
      WHEELS_UNREACHABLE();
  }
}

void Simulator::LogWake(Thread* waker, Thread* waiter, WakeType wake_type) {
  log::EventBuilder builder;

  {
    auto descr = builder.DescrWriter();

    descr.Append("Wake thread #{}", waiter->id);
    if (waker != nullptr) {
      descr.Append(" from thread #{}", waker->id);
    }
    descr.Append(", reason: {}", WakeTypeToString(wake_type));
  }

  builder.Done();

  Log(builder.EventView());
}

// From simulator context
void Simulator::Wake(Thread* waker, Thread* waiter, WakeType wake) {
  WHEELS_ASSERT(waiter->state == ThreadState::Parked || waiter->state == ThreadState::Sleeping,
                "Unexpected thread state");

  call::Status status;

  switch (wake) {
    case WakeType::SleepTimer:
      // SysSleepUntil -> Timeout
      status = call::Status::Ok;
      break;

    case WakeType::FutexWake:
      status = call::Status::Ok;
      memory_model_.Wake(waker, waiter);
      if (waiter->timer != nullptr) {
        // SysFutexWaitTimed -> SysFutexWake
        CancelTimer(waiter->timer);
      } else {
        // Do nothing
      }
      break;

    case WakeType::FutexTimer:
      // SysFutexWaitTimed -> Timeout
      status = call::Status::Timeout;
      waiter->futex->waiters.Remove(waiter);
      break;

    case WakeType::Spurious:
      // Spurious wake-up
      status = call::Status::Interrupted;
      if (waiter->futex != nullptr) {
        waiter->futex->waiters.Remove(waiter);
      }
      if (waiter->timer != nullptr) {
        CancelTimer(waiter->timer);
      }

      break;
  }

  {
    // Reset thread state

    waiter->state = ThreadState::Runnable;

    waiter->futex = nullptr;
    waiter->timer = nullptr;
    waiter->waiter = nullptr;
  }

  if (IsSysLoggingEnabled()) {
    LogWake(waker, waiter, wake);
  }

  ScheduleWaiter(waiter, status);
}

Thread* Simulator::CreateThread(IThreadUserState* state) {
  ++thread_count_;

  auto id = ids_.AllocateId();

  auto stack = memory_.AllocateStack();
  if (params_.memset_stacks) {
    MemsetStack(stack.MutView(), /*chr=*/*params_.memset_stacks);
  }

  // Kernel-space allocation
  Thread* thread = new Thread{};
  thread->Reset(state, std::move(stack), id);

  alive_.PushBack(thread);

  return thread;
}

void Simulator::DestroyThread(Thread* thread) {
  alive_.Unlink(thread);

  memory_.FreeStack(std::move(*(thread->stack)));
  ids_.Free(thread->id);

  delete thread;
}

void Simulator::Burn(Thread* thread) {
  {
    RunQueue()->Remove(thread);
    if (thread->timer) {
      timer_queue_.Remove(thread->timer);
    }
    if (thread->futex) {
      thread->futex->waiters.Remove(thread);
    }
  }

  ++burnt_threads_;

  DestroyThread(thread);
}

void Simulator::StopLoop() {
  WHEELS_VERIFY(RunQueue()->IsIdle(), "Broken scheduler");
  WHEELS_VERIFY(timer_queue_.IsEmpty(), "Broken scheduler");

  done_ = true;

  memory_model_.Reset();

  futex_.clear();

  main_.reset();

  ResetCurrentScheduler();
}

void Simulator::CheckMemory() {
#if defined(__TWIST_SIM_ISOLATION__)
  if (burnt_threads_ > 0) {
    return;  // Incompatible with memory leaks check
  }

  auto stat = memory_.HeapStat();

  if (stat.alloc_count != stat.free_count) {
    size_t bytes_leaked = stat.user_bytes_allocated - stat.user_bytes_freed;
    size_t leak_count = stat.alloc_count - stat.free_count;

    stderr_ << "Memory leaks: "
        << bytes_leaked << " byte(s) leaked" << " in " << leak_count << " allocation(s)" << std::endl;

    auto& allocations = memory_model_.Allocations();
    for (const auto& [addr, alloc] : allocations) {
      if (alloc.alive) {
        if (alloc.source_loc) {
          stderr_ << ::fmt::format("- address = {}, size = {}, thread = #{}, context = {}",
                                   fmt::ptr(addr), alloc.size, alloc.thread_id, *alloc.source_loc) << std::endl;
        } else {
          stderr_ << ::fmt::format("- address = {}, size = {}, thread = #{}",
                                   fmt::ptr(addr), alloc.size, alloc.thread_id) << std::endl;
        }
      }
    }

    _SysAbort(Status::MemoryLeak);
  }
#endif
}

bool Simulator::IsDeadlock() const {
  return RunQueue()->IsIdle() &&
         timer_queue_.IsEmpty() &&
         alive_.NonEmpty();
}

void Simulator::CheckDeadlock() {
  if (IsDeadlock()) {
    // Validate
    for (Thread* f : alive_) {
      WHEELS_VERIFY(f->state == ThreadState::Parked ||
                        f->state == ThreadState::LfSuspended,
                    "Internal error");
    }
    ReportDeadlock();
    _SysAbort(Status::Deadlock);
  }
}

void Simulator::ReportDeadlock() {
  stderr_ << "Deadlock detected: "
          << alive_.Size() << " threads\n";

  stderr_ << fmt::format("Scheduler loop iterations: {}\n", iter_);
  if (last_checkpoint_ != 0) {
    stderr_ << fmt::format("Last checkpoint: iteration #{}\n", last_checkpoint_);
  }

  stderr_ << std::endl;  // Blank line

  for (Thread* thread : alive_) {
    if (thread->state == ThreadState::Parked) {
      // Parked on futex
      thread->state = ThreadState::Deadlocked;
      running_ = thread;
      SwitchTo(thread);
      running_ = nullptr;
      std::string_view report = thread->report;
      stderr_.write(report.begin(), report.size());
    } else if (thread->state == ThreadState::LfSuspended) {
      // Suspended by scheduler
      stderr_ << fmt::format("Thread #{} is suspended before {} ({})\n",
                             thread->id,
                             thread->action->operation,
                             thread->action->source_loc);
    } else {
      std::abort();  // Unreachable
    }
  }
}

void Simulator::ReportFromDeadlockedThread(Thread* me) {
  // User-space

  auto status = user::fmt::TempBufWriter();

  status.Append("Thread #{} is blocked at {} ({})", me->id,
                me->waiter->operation, me->waiter->source_loc);

  if (me->waiter->waiting_for) {
    status.Append(", waiting for thread #{}", *(me->waiter->waiting_for));
  }
  status.Append("\n");

  // Stack trace

  /*
  std::ostringstream trace_out;
  PrintStackTrace(trace_out);
  const auto trace = trace_out.str();
  if (!trace.empty()) {
    report_out << trace << std::endl;
  }
  */

  me->report = status.StringView();

  me->context.SwitchTo(loop_context_);  // Never returns

  WHEELS_UNREACHABLE();
}

void Simulator::SpuriousWakeups() {
  // TODO
}

bool Simulator::DeterministicMemory() const {
#if (__TWIST_SIM_ISOLATION__)
  return memory_.FixedMapping();
#else
  return false;
#endif
}

bool Simulator::DigestAtomicLoads() const {
  if (params_.digest_atomic_loads) {
    return true;  // Explicit directive
  }

  // Conservative decision
  return params_.allow_digest_atomic_loads &&
         DeterministicMemory();
}

bool Simulator::IsSysLoggingEnabled() const {
  if (logger_ == nullptr) {
    return false;
  }

  if (!allow_sys_logging_) {
    return false;
  }

  if (AmIUser()) {
    trace::Scope* scope = trace::rt::TryCurrentScope();
    if ((scope != nullptr) && !scope->IsEventVisible()) {
      return false;
    }
  }

  return true;
}

void Simulator::Log(log::Event* event) {
  if (logger_ != nullptr) {
    logger_->Add(event);
  }
}

void Breakpoint(Thread* thread) {
  (void)thread;
}

void Simulator::DebugStart(Thread* thread) {
  Breakpoint(thread);
}

void Simulator::DebugInterrupt(Thread* thread) {
  Breakpoint(thread);
}

void Simulator::DebugSwitch(Thread* thread) {
  Breakpoint(thread);
}

void Simulator::DebugResume(Thread* thread) {
  Breakpoint(thread);
}

size_t ThreadCount() {
  return Simulator::Current()->ThreadCount();
}

void AbortIter(Status status, std::string why) {
  Simulator::Current()->AbortIter(status, std::move(why));
}

}  // namespace system

}  // namespace twist::rt::sim
