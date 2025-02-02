#pragma once

#include "domain.hpp"
#include "scope.hpp"
#include "event.hpp"

namespace twist::trace::rt {

struct NewScope {
  Scope* parent;
  Id id;
};

NewScope ScopeEnter(Scope*);
void ScopeExit(Scope* curr, Scope* parent);

Scope* TryCurrentScope();

void Trace(Event*);

}  // namespace twist::trace::rt
