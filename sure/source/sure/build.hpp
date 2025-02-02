#pragma once

#include <sure/machine/context.hpp>

namespace sure::build {

constexpr bool StackPointerAvailable() {
  return MachineContext::kStackPointerAvailable;
}

}  // namespace sure::build
