#pragma once

#include <sure/trampoline.hpp>
#include <sure/stack_view.hpp>

extern "C" {
#include "context.h"
}

namespace sure {

// Target architecture: armv8-a

struct MachineContext {
  void* rsp_;

  void Setup(StackView stack, ITrampoline* trampoline);

  void SwitchTo(MachineContext& target);

  static constexpr bool kStackPointerAvailable = true;

  void* StackPointer() const noexcept {
    return rsp_;
  }
};

}  // namespace sure
