#include <twist/trace/runtime.hpp>

#include <twist/rt/thr/fiber/assist.hpp>
#include <twist/rt/thr/logging/logging.hpp>

#include <wheels/core/assert.hpp>

#include <atomic>
#include <utility>

namespace thr_fiber = twist::rt::thr::fiber;
namespace thr_log = twist::rt::thr::log;

namespace twist::trace::rt {

void Trace(Event* event) {
  thr_log::LogMessage(event->scope, event->loc, event->message);
}

class IdGenerator {
 public:
  Id GenerateId() noexcept {
    return next_id_.fetch_add(1, std::memory_order::relaxed);
  }

 private:
  std::atomic_uint64_t next_id_ = 0;
};

static Id GenerateId() {
  static IdGenerator ids;
  return ids.GenerateId();
}

static Scope* SetScope(Scope* scope) {
  auto* fiber = thr_fiber::CurrentFiber();
  WHEELS_VERIFY(fiber != nullptr, "Invalid fiber");
  return std::exchange(fiber->scope, scope);
}

NewScope ScopeEnter(Scope* scope) {
  return {SetScope(scope), GenerateId()};
}

void ScopeExit(Scope* curr, Scope* parent) {
  Scope* prev = SetScope(parent);
  WHEELS_VERIFY(curr == prev, "ScopeExit: unexpected current scope");
}

Scope* TryCurrentScope() {
  auto* fiber = thr_fiber::CurrentFiber();
  WHEELS_VERIFY(fiber != nullptr, "Invalid fiber");
  return fiber->scope;
}

}  // namespace twist::trace::rt
