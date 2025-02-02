#pragma once

#include <twist/rt/sim/system/params.hpp>
#include <twist/rt/sim/system/main.hpp>
#include <twist/rt/sim/system/thread/struct.hpp>
#include <twist/rt/sim/system/thread/entry.hpp>
#include <twist/rt/sim/system/digest.hpp>
#include <twist/rt/sim/system/time.hpp>
#include <twist/rt/sim/system/futex/loc.hpp>
#include <twist/rt/sim/system/futex/waiter.hpp>
#include <twist/rt/sim/system/futex/waker.hpp>
#include <twist/rt/sim/system/futex/wait_queue.hpp>
#include <twist/rt/sim/system/futex/futex.hpp>
#include <twist/rt/sim/system/futex/wake_type.hpp>
#include <twist/rt/sim/system/timer/timer_queue.hpp>
#include <twist/rt/sim/system/id_allocator.hpp>
#include <twist/rt/sim/system/result.hpp>
#include <twist/rt/sim/system/call/handler.hpp>
#include <twist/rt/sim/system/call/status.hpp>
#include <twist/rt/sim/system/call/context.hpp>
#include <twist/rt/sim/system/memory.hpp>
#include <twist/rt/sim/system/scheduler/scheduler.hpp>
#include <twist/rt/sim/system/status.hpp>

#include <twist/rt/sim/system/sync/action.hpp>
#include <twist/rt/sim/system/sync/model.hpp>

#include <twist/rt/sim/system/log/event.hpp>
#include <twist/rt/sim/system/log/logger.hpp>

#include <twist/trace/event.hpp>

#include <sure/context.hpp>

#include <wheels/core/assert.hpp>

#include <chrono>
#include <optional>
#include <string_view>
#include <sstream>
#include <random>
#include <map>
#include <utility>

namespace twist::rt::sim {

namespace system {

class Simulator
    : private IThreadUserState {
  friend void Entry(Thread*);

  friend struct SleepTimer;
  friend struct FutexTimer;

 public:
  using Params = system::Params;
  using Result = system::Result;
  using Status = system::Status;
  using Digest = uint64_t;

 public:
  Simulator(IScheduler* scheduler, Params params = Params());

  IScheduler* Scheduler() const  {
    return scheduler_;
  }

  // One-shot
  Result Run(MainRoutine);

  // Debugger mode
  void Start(MainRoutine);
  size_t RunFor(size_t iters);
  bool RunOneIter();
  Result Drain();
  Result Burn();

  uint64_t GetDigest() const {
    return digest_.Digest();
  }

  static Simulator* Current() {
    WHEELS_VERIFY(current, "Not in simulator");
    return current;
  }

  // TODO: Workaround, remove later
  static Simulator* TryCurrent() {
    return current;
  }

  // System calls (Sys{Name})

  // To measure overhead
  void SysNoop();

  ThreadId SysSpawn(IThreadUserState*, const UserContext*);
  ThreadId SysGetId();
  void SysDetach(ThreadId);  // Workaround for check

  void SysYield(const UserContext*);

  void SysSwitchTo(ThreadId);

  call::Status SysSleepFor(Time::Duration);
  call::Status SysSleepUntil(Time::Instant);

  uint64_t SysSync(sync::Action* action);
  void SysAccessNonAtomicVar(sync::Access* access);

  // Futex
  call::Status SysFutexWait(FutexLoc loc, uint64_t old, const WaiterContext*);
  call::Status SysFutexWaitTimed(FutexLoc loc, uint64_t old, Time::Instant, const WaiterContext*);
  void SysFutexWake(FutexLoc, size_t, const WakerContext*);

  void SysMalloc(void* addr, size_t size);
  void SysFree(void* addr);

  Time::Instant SysNow() const {
    // Optimize SystemCall: Allocation-free
    return time_.Now();
  }

  uint64_t SysRandomNumber();
  size_t SysRandomChoice(size_t alts);

  bool SysSpuriousTryFailure();

  void SysWrite(int fd, std::string_view buf);

  [[noreturn]] void SysAbort(Status);

  bool SysSetThreadAttrPreemptive(bool on);
  bool SysSetThreadAttrLockFree(bool on);

  // Hints for scheduler
  void SysSchedHintProgress();
  void SysSchedHintNewIter();

  void SysThreadEnter(user::fiber::Fiber*);
  [[noreturn]] void SysThreadExit();

  // Fiber support
  void SysResetFiberStack(std::span<std::byte> stack);
  uint64_t SysRegisterFiber(user::fiber::Fiber*);
  user::fiber::Fiber* SysSwitchToFiber(user::fiber::Fiber*);
  user::fiber::Fiber* SysCurrentFiber();

  void SysLog(log::Event*);

  uint64_t SysNewScopeId();
  void SysTrace(trace::Event*);

  bool AmIUser() const {
    return running_ != nullptr;
  }

  bool ForceGlobalAllocator() const {
    // Exceptions
    return false;
  }

  bool UserMemoryAllocator() const {
    return AmIUser() && !ForceGlobalAllocator();
  }

  void UserInitMalloc() {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    memory_.InitMalloc();
  }

  void* UserMalloc(size_t size) {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    WHEELS_VERIFY(allow_alloc_, "Allocation under UserAllocGuard");
    void* addr = memory_.Malloc(size);
    SysMalloc(addr, size);
    return addr;
  }

  void UserFree(void* ptr) {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    memory_.Free(ptr);
    SysFree(ptr);
  }

  void* UserAllocStatic(size_t size) {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    return memory_.AllocateStatic(size);
  }

  void UserDeallocStatics() {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    memory_.DeallocateStatics();
  }

  bool UserMemoryAccess(void* addr, size_t size) {
    return memory_.Access(addr, size);
  }

  Thread* RunningThread() const {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    return running_;
  }

  user::tls::Storage& UserTls() {
    WHEELS_ASSERT(AmIUser(), "Not in user-space");
    return RunningThread()->tls;
  }

  void Silent(bool on) {
    silent_ = on;
  }

  void SetLogger(log::ILogger* logger) {
    logger_ = logger;
  }

  void AllowSysLogging(bool on) {
    allow_sys_logging_ = on;
  }

  void Debug(bool on) {
    debug_ = on;
  }

  // Statistics for tests
  size_t IterCount() const {
    return iter_;
  }

  size_t ThreadCount() const {
    return thread_count_;
  }

  // Statistics for tests

  size_t FutexWaitCount() const {
    return futex_wait_count_;
  }

  size_t FutexWakeCount() const {
    return futex_wake_count_;
  }

  size_t FutexCount() const {
    return FutexWaitCount() + FutexWakeCount();
  }

  // Statistics for tests
  size_t YieldCount() const {
    return yield_count_;
  }

  void Checkpoint() {
    last_checkpoint_ = iter_;
  }

  void DigestAtomicLoad(uint64_t repr);

  // Scheduler context

  uint64_t CurrTick() const {
    return ticker_.Count();
  }

  size_t CurrIter() const {
    return iter_;
  }

  // From kernel
  void Prune(std::string why) {
    stderr_ << "Simulation aborted by scheduler: " << why << std::endl;
    _SysAbort(Status::Pruned);
  }

  void AbortIter(Status status, std::string err) {
    stderr_ << err << std::endl;
    _SysAbort(status);
  }

  sync::SeqCstMemoryModel& SharedMemory() {
    return memory_model_;
  }

  size_t SpinWaitYieldThreshold() const {
    return params_.spin_wait_yield_threshold;
  }

  size_t HardwareConcurrency() const {
    return params_.hardware_concurrency;
  }

  Memory& UserMemory() {
    return memory_;
  }

  bool AllowUserAlloc(bool flag) {
    return std::exchange(allow_alloc_, flag);
  }

 private:
  void ValidateParams();

  void Init();

  void SetCurrentScheduler();
  void ResetCurrentScheduler();

  // IThreadUserState
  void RunUser() override;
  void AtThreadExit() noexcept override;
  wheels::SourceLocation SourceLoc() const noexcept override;

  void SpawnMainThread(MainRoutine);
  void MainCompleted();

  void StartLoop(MainRoutine);
  // Return number of iterations made
  size_t RunLoop(size_t iter_budget);
  void StopLoop();

  IRunQueue* RunQueue() const;

  void Iter();
  void Maintenance();
  bool Idle();

  void SpuriousWakeups();

  bool IsDeadlock() const;
  void CheckDeadlock();
  void ReportFromDeadlockedThread(Thread*);
  void ReportDeadlock();

  void CheckMemory();

  void AddTimer(Timer*);
  void CancelTimer(Timer*);
  // true means progress
  bool PollTimers();

  // Advance virtual time
  void Tick();

  // Debugging
  void DebugStart(Thread*);
  void DebugInterrupt(Thread*);
  void DebugSwitch(Thread*);
  void DebugResume(Thread*);

  // Context switches

  // Scheduler -> thread
  void SwitchTo(Thread*);
  // Running thread -> scheduler
  call::Status SystemCall(call::Handler handler);

  // Context switch: scheduler -> thread
  void Run(Thread*);
  void CompleteSync(Thread*);
  void Iter(Thread*);
  // true - resume immediately, false - suspend

  void CheckStackPointer(Thread*);
  bool HandleSystemCall(Thread*);

  Thread* CreateThread(IThreadUserState* state);
  void DestroyThread(Thread*);

  void Burn(Thread*);
  void BurnDetachedThreads();

  // Add thread to run queue
  Futex* GetFutex(FutexLoc);
  void ScheduleWaiter(Thread*, call::Status status = call::Status::Ok);
  void Wake(Thread* waker, Thread* waiter, WakeType);

  uint64_t NextFiberId() {
    return ++next_fiber_id_;
  }

  // Decision
  bool DigestAtomicLoads() const;
  bool DeterministicMemory() const;

  // System call handlers

  // NOLINTBEGIN
  ThreadId _SysSpawn(Thread* parent, IThreadUserState*);
  void _SysDetach(ThreadId id);
  bool _SysYield(Thread*);
  bool _SysSwitchTo(Thread*, ThreadId);
  void _SysSleep(Thread*, Timer*);
  bool _SysSync(Thread*, sync::Action*);
  void _SysAccessNonAtomicVar(Thread*, sync::Access*);
  void _SysFutexWait(Thread*, FutexLoc loc, uint64_t old, Timer*, const WaiterContext*);
  void _SysFutexWake(Thread*, FutexLoc loc, size_t);
  void _SysMalloc(Thread*, void* addr, size_t size);
  void _SysFree(Thread*, void* addr);
  void _SysLog(Thread*, wheels::SourceLocation loc, std::string_view message);
  void _SysWrite(int fd, std::string_view buf);
  void _SysAbort(Status);
  void _SysThreadEnter(Thread*, user::fiber::Fiber*);
  void _SysThreadExit(Thread*);
  user::fiber::Fiber* _SysSwitchToFiber(Thread*, user::fiber::Fiber*);
  uint64_t _SysRegisterFiber(Thread*, user::fiber::Fiber*);
  void _SysLog(Thread*, log::Event*);
  // NOLINTEND

  //

  uint64_t Sync(Thread*, sync::Action*);

  bool IsSysLoggingEnabled() const;
  void Log(log::Event*);

 private:
  // System
  void LogPrevThreadStatus(ThreadId);
  void LogIter(Thread*);

  void LogSysSpawn(Thread*, const UserContext*, ThreadId);

  // User
  void LogSysYield(Thread*, wheels::SourceLocation source_loc);

  // User
  void LogSysSync(Thread*, sync::Action*);

  // System
  void LogWake(Thread* waker, Thread* waiter, WakeType);

  // User
  void LogSysFutexWaitEnter(Thread*, FutexLoc, uint64_t old, const WaiterContext* ctx);
  void LogSysFutexWaitExit(Thread*, FutexLoc, uint64_t old, const WaiterContext* ctx);
  void LogSysFutexWake(Thread* caller, FutexLoc loc, size_t count, const WakerContext*);

  // User
  void LogSysWrite(int fd, std::string_view buf);

  // User
  void LogSysSwitchToFiber(Thread*, user::fiber::Fiber* target_fiber);

 private:
  const Params params_;

  IScheduler* scheduler_;
  std::optional<ThreadId> last_thread_;

  Ticker ticker_;
  Time time_;

  Memory memory_;
  bool allow_alloc_ = true;

  IdAllocator ids_;

  DigestCalculator digest_;
  // Cached
  bool digest_atomic_loads_{false};

  std::optional<MainRoutine> main_;

  wheels::IntrusiveList<Thread, AliveTag> alive_;

  size_t burnt_threads_ = 0;

  sure::ExecutionContext loop_context_;  // Thread context
  size_t iter_{0};
  // For debugger
  size_t last_checkpoint_{0};

  // Run queue
  // IRunQueue* RunQueue();
  Thread* running_{nullptr};
  std::optional<ThreadId> prev_thread_id_;

  sync::SeqCstMemoryModel memory_model_;

  std::map<FutexKey, Futex> futex_;

  uint64_t next_fiber_id_ = 0;
  uint64_t next_scope_id_ = 0;

  // Timer queue
  TimerQueue timer_queue_;

  call::Handler syscall_;

  size_t step_index_ = 0;

  bool done_{false};

  std::stringstream stdout_;
  std::stringstream stderr_;

  std::optional<Status> status_;
  std::optional<FailureContext> failure_context_;

  log::ILogger* logger_ = nullptr;
  bool allow_sys_logging_ = false;

  // For logging
  bool silent_{false};
  bool debug_{false};

  // Statistics
  size_t futex_wait_count_{0};
  size_t futex_wake_count_{0};
  size_t yield_count_{0};

  size_t thread_count_{0};

  static Simulator* current;
};

}  // namespace system

}  // namespace twist::rt::sim
