#pragma once

#include "domain.hpp"
#include "scope.hpp"
#include "loc.hpp"

#include <string_view>

namespace twist::trace {

struct Event {
  Scope* scope;
  Loc loc;
  std::string_view message;
};

}  // namespace twist::trace
