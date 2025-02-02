#include "scope.hpp"

#include "runtime.hpp"

namespace twist::trace {

Scope::Scope(const Domain& domain, const char* name, Loc loc)
    : name_(name), loc_(loc) {
  domain_ = const_cast<Domain*>(&domain);

  auto enter = rt::ScopeEnter(this);

  parent_ = enter.parent;
  id_ = enter.id;

  if (parent_ != nullptr) {
    // Inherit context_
    context_ = parent_->GetContext();
  }
}

Scope::Scope(const char* name, Loc loc)
    : name_(name), loc_(loc) {

  auto enter = rt::ScopeEnter(this);

  parent_ = enter.parent;
  id_ = enter.id;

  if (parent_ != nullptr) {
    // Inherit domain & context
    domain_ = parent_->GetDomain();
    context_ = parent_->GetContext();
  } else {
    domain_ = Domain::Global();
  }
}

Scope* Scope::TryCurrent() noexcept {
  return rt::TryCurrentScope();
}

void Scope::Trace(std::string_view message, Loc loc) {
  if (IsEventVisible()) {
    Event event{this, loc, message};
    rt::Trace(&event);
  }
}

int Scope::GetEventVisibility() const noexcept {
  int ctx = GetVisibility();
  int dom = domain_->GetVisibility();
  return ctx + dom;
}

Scope::~Scope() {
  // Rollback
  rt::ScopeExit(this, parent_);
}

}  // namespace twist::trace
