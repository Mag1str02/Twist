#pragma once

#include <variant>

namespace wheels {

// https://en.wikipedia.org/wiki/Unit_type
// https://github.com/facebook/folly/blob/master/folly/Unit.h

using Unit = std::monostate;

}  // namespace wheels
