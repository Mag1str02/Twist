#include "struct.hpp"

#include "../simulator.hpp"

#include <twist/rt/sim/user/fiber/fiber.hpp>

#include <twist/rt/sim/user/fmt/temp.hpp>
#include <twist/rt/sim/user/library/system/write.hpp>

#include <twist/rt/sim/user/static/manager.hpp>
#include <twist/rt/sim/user/static/thread_local/manager.hpp>

#include <twist/rt/sim/user/scheduler/preemption.hpp>

#include <twist/trace/scope.hpp>
#include <twist/trace/attr/uint.hpp>
#include <twist/trace/attr/unit.hpp>

#include <twist/wheels/fmt/source_location.hpp>

namespace twist::rt::sim {

namespace system {

[[noreturn]] void AbortOnUnhandledException(Thread* me) {
  auto status = user::fmt::TempBufWriter();

  status.Append("Unhandled user exception in thread #{}", me->id);
  if (me->main) {
    status.Append(" (main thread)");
  } else {
    status.Append(" ({})", me->user->SourceLoc());
  }

  user::library::system::Write(2, status.StringView());
  Simulator::Current()->SysAbort(Status::UnhandledException);  // Never returns
}

void Entry(Thread* me) {
  Simulator* simulator = Simulator::Current();

  user::fiber::Fiber thread_fiber;
  simulator->SysThreadEnter(&thread_fiber);

  if (me->main) {
    simulator->UserInitMalloc();
  }

  trace::Scope scope("Thread");
  trace::attr::Uint id_attr{"id", me->id};
  trace::attr::Unit rt_tag{"twist.rt"};
  scope.LinkAttr(id_attr).LinkAttr(rt_tag);

  scope.SetVisibility(1);

  if (me->main) {
    // Static global variables
    user::ss::Manager::Instance().ConstructGlobalVars();
  }

  try {
    me->user->RunUser();
  } catch (...) {
    AbortOnUnhandledException(me);  // Never returns
  }

  // Thread-local variables
  user::tls::Manager::Instance().Destroy(me->tls);

  if (me->main) {
    // TODO: Source location?
    // scope.Note("Exiting main");
  } else {
    scope.Note("Thread #{} completed", me->id, trace::NoteCtx{me->user->SourceLoc()});
  }

  {
    user::scheduler::PreemptionGuard guard{};
    me->user->AtThreadExit();
  }

  // Static variables
  if (me->main) {
    user::ss::Manager::Instance().DestroyVars();
  }

  simulator->SysThreadExit();  // Never returns

  WHEELS_UNREACHABLE();
}

}  // namespace system

}  // namespace twist::rt::sim
