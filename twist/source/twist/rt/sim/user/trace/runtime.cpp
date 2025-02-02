#include <twist/trace/runtime.hpp>

#include <twist/rt/sim/user/fiber/assist.hpp>
#include <twist/rt/sim/user/syscall/trace.hpp>

#include <wheels/core/assert.hpp>

#include <utility>

namespace user = twist::rt::sim::user;

namespace twist::trace::rt {

void Trace(Event* event) {
  user::syscall::Trace(event);
}

static Scope* SetScope(Scope* scope) {
  auto* fiber = user::fiber::CurrentFiber();
  WHEELS_VERIFY(fiber != nullptr, "Invalid fiber");
  return std::exchange(fiber->scope, scope);
}

NewScope ScopeEnter(Scope* scope) {
  return {SetScope(scope), user::syscall::NewScopeId()};
}

void ScopeExit(Scope* curr, Scope* parent) {
  Scope* prev = SetScope(parent);
  WHEELS_VERIFY(curr == prev, "ScopeExit: unexpected current scope");
}

Scope* TryCurrentScope() {
  auto* fiber = user::fiber::CurrentFiber();
  WHEELS_VERIFY(fiber != nullptr, "Invalid fiber");
  return fiber->scope;
}

}  // namespace twist::trace::rt
