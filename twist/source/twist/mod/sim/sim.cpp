#include "det.hpp"
#include "explore.hpp"
#include "test.hpp"
#include "print.hpp"

#include "stat/atomic.hpp"
#include "stat/futex.hpp"
#include "stat/memory.hpp"

#include "sched/coop.hpp"
#include "sched/fair.hpp"
#include "sched/random.hpp"
#include "sched/pct.hpp"
#include "sched/dfs.hpp"

#include <wheels/core/panic.hpp>

#include <random>

#if defined(__TWIST_SIM_ISOLATION__)
#include <twist/rt/sim/system/memory/isolated/static.hpp>
#endif

#include <twist/rt/sim/user/syscall/switch_to.hpp>

namespace twist::sim {

//

static Digest DetCheckSim(SimulatorParams params, MainRoutine main, size_t index) {
  static const size_t kIterations = 1024;

  static unsigned char kMemsetByte[3] = {1, 0xFF, 0b10101011};

  params.memset_stacks = kMemsetByte[index % 3];
  // TODO: big allocations (e.g. fiber stacks)
  // params.memset_malloc = kMemsetByte[index % 3];

  sched::RandomScheduler scheduler{};
  Simulator simulator{&scheduler, params};

  simulator.Silent(true);
  simulator.Start(main);
  simulator.RunFor(kIterations);
  auto result = simulator.Burn();

  return result.digest;
}

bool DetCheck(SimulatorParams params, MainRoutine main) {
  {
    /*
     * /\ Isolated user memory
     * /\ Same memory mapping for same process
     */
    params.digest_atomic_loads = true;
    // Do not crash in determinism check
    params.crash_on_abort = false;
    params.forward_stdout = false;
  }

  Digest d1 = DetCheckSim(params, main, 0);
  Digest d2 = DetCheckSim(params, main, 1);

  if (d1 != d2) {
    return false;
  }

  Digest d3 = DetCheckSim(params, main, 2);

  if (d3 != d1) {
    return false;
  }

  return true;
}

//

namespace sched {

Schedule Record(IScheduler& scheduler, SimulatorParams params, MainRoutine main, Digest digest) {
  Recorder recorder{&scheduler};
  Simulator simulator{&recorder, params};
  auto result = simulator.Run(main);
  WHEELS_VERIFY(result.digest == digest, "Digest mismatch for recorder");

  auto schedule = recorder.GetSchedule();

  {
    // Test replay
    ReplayScheduler replay_scheduler{schedule};
    Simulator replay_simulator{&replay_scheduler, params};
    auto replay_result = replay_simulator.Run(main);
    WHEELS_VERIFY(replay_result.digest == digest, "Digest mismatch for replay");
  }

  return recorder.GetSchedule();
}

}  // namespace sched

//

ExploreResult Explore(IScheduler& scheduler, SimulatorParams params, MainRoutine main) {
  if (!DetCheck(params, main)) {
    wheels::Panic("Simulation is not deterministic");
  }

  {
    params.forward_stdout = false;
    params.crash_on_abort = false;
  }

  size_t count = 0;

  do {
    Simulator sim{&scheduler, params};

    auto result = sim.Run(main);
    ++count;

    if (result.Failure()) {
      auto replay_schedule = sched::Record(scheduler, params, main, result.digest);

      return ExploreResult{
          .found ={{replay_schedule, result}},
          .sims=count,
      };
    }
  } while (scheduler.NextSchedule());

  return {{}, count};
}

ExploreResult Explore(IScheduler& scheduler, MainRoutine main) {
  return Explore(scheduler, {}, main);
}

void Print(MainRoutine main, sched::Schedule schedule, SimulatorParams params) {
  sched::ReplayScheduler scheduler{schedule};
  Simulator simulator{&scheduler, params};
  simulator.Silent(true);
  log::PrettyPrinter pretty_printer;
  simulator.SetLogger(&pretty_printer);
  simulator.AllowSysLogging(true);
  auto result = simulator.Run(main);
}

Result TestSim(SimulatorParams params, MainRoutine main) {
  assert(DetCheck(params, main));

  sched::FairScheduler fair_scheduler{};
  Simulator sim{&fair_scheduler, params};
  return sim.Run(main);
}

void SwitchTo(std::uint8_t thread_id) {
  twist::rt::sim::user::syscall::SwitchTo(thread_id);
}

void AssignMemoryRange(void* addr, size_t size) {
#if defined(__TWIST_SIM_ISOLATION__)
  twist::rt::sim::system::memory::isolated::StaticMemoryMapper()->Assign(addr, size);
#else
  // Unused
  (void)addr;
  (void)size;

  wheels::Panic("Not supported, set TWIST_SIM_ISOLATION CMake flag");
#endif
}

// Statistics

static rt::sim::system::Simulator* CurrentSimulator() {
  return rt::sim::system::Simulator::Current();
}

namespace stat {

// Atomic

size_t AtomicCount() {
  return CurrentSimulator()->SharedMemory().AtomicCount();
}

// Futex

size_t FutexWaitSystemCallCount() {
  return CurrentSimulator()->FutexWaitCount();
}

size_t FutexWakeSystemCallCount() {
  return CurrentSimulator()->FutexWakeCount();
}

// Memory

size_t AllocationCount() {
#if defined(__TWIST_SIM_ISOLATION__)
  return CurrentSimulator()->UserMemory().HeapStat().alloc_count;
#else
  WHEELS_PANIC("Not supported in this build");
#endif
}

size_t TotalBytesAllocated() {
#if defined(__TWIST_SIM_ISOLATION__)
  return CurrentSimulator()->UserMemory().HeapStat().user_bytes_allocated;
#else
  WHEELS_PANIC("Not supported in this build");
#endif
}

size_t WorkingSetInBytes() {
#if defined(__TWIST_SIM_ISOLATION__)
  auto stat = CurrentSimulator()->UserMemory().HeapStat();
  return stat.user_bytes_allocated - stat.user_bytes_freed;
#else
  WHEELS_PANIC("Not supported in this build");
#endif
}

}  // namespace stat

}  // namespace twist::sim
