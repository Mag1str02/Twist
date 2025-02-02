#include <twist/rt/sim/user/syscall/spawn.hpp>
#include <twist/rt/sim/user/syscall/abort.hpp>
#include <twist/rt/sim/user/syscall/yield.hpp>
#include <twist/rt/sim/user/syscall/sleep.hpp>
#include <twist/rt/sim/user/syscall/futex.hpp>
#include <twist/rt/sim/user/syscall/now.hpp>
#include <twist/rt/sim/user/syscall/random.hpp>
#include <twist/rt/sim/user/syscall/id.hpp>
#include <twist/rt/sim/user/syscall/trace.hpp>
#include <twist/rt/sim/user/syscall/tls.hpp>

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user::syscall {

void Noop() {
  system::Simulator::Current()->SysNoop();
}

system::ThreadId Spawn(system::IThreadUserState* state, const system::UserContext* ctx) {
  return system::Simulator::Current()->SysSpawn(state, ctx);
}

void Detach(system::ThreadId id) {
  system::Simulator::Current()->SysDetach(id);
}

uint64_t Sync(system::sync::Action* action) {
  return system::Simulator::Current()->SysSync(action);
}

void Yield(const system::UserContext* ctx) {
  system::Simulator::Current()->SysYield(ctx);
}

void SwitchTo(system::ThreadId target) {
  system::Simulator::Current()->SysSwitchTo(target);
}

void SleepFor(system::Time::Duration delay) {
  system::Simulator::Current()->SysSleepFor(delay);
}

void SleepUntil(system::Time::Instant deadline) {
  system::Simulator::Current()->SysSleepUntil(deadline);
}

system::call::Status FutexWait(system::FutexLoc loc, uint64_t val, system::WaiterContext* ctx) {
  return system::Simulator::Current()->SysFutexWait(loc, val, ctx);
}

system::call::Status FutexWaitTimed(system::FutexLoc loc, uint64_t val, system::Time::Instant d, system::WaiterContext* ctx) {
  return system::Simulator::Current()->SysFutexWaitTimed(loc, val, d, ctx);
}

void FutexWake(system::FutexLoc loc, size_t count, system::WakerContext* ctx) {
  system::Simulator::Current()->SysFutexWake(loc, count, ctx);
}

system::ThreadId GetId() {
  return system::Simulator::Current()->SysGetId();
}

system::Time::Instant Now() {
  return system::Simulator::Current()->SysNow();
}

uint64_t RandomNumber() {
  return system::Simulator::Current()->SysRandomNumber();
}

size_t RandomChoice(size_t alts) {
  return system::Simulator::Current()->SysRandomChoice(alts);
}

void Abort(system::Status status) {
  system::Simulator::Current()->SysAbort(status);
}

void Write(int fd, std::string_view buf) {
  system::Simulator::Current()->SysWrite(fd, buf);
}

trace::Id NewScopeId() {
  return system::Simulator::Current()->SysNewScopeId();
}

void Trace(trace::Event* event) {
  system::Simulator::Current()->SysTrace(event);
}

user::tls::Storage& AccessTls() {
  return system::Simulator::Current()->UserTls();
}

}  // namespace user::syscall

}  // namespace twist::rt::sim
