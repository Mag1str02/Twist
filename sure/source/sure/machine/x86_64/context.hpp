#pragma once

#include <sure/trampoline.hpp>
#include <sure/stack_view.hpp>

extern "C" {
#include "context.h"
}

namespace sure {

// Target architecture: x86-64

struct MachineContext {
  void* rsp_;

  void Setup(StackView stack, ITrampoline* trampoline);

  void SwitchTo(MachineContext& target) {
    SwitchMachineContext(&rsp_, &target.rsp_);
  }

  static constexpr bool kStackPointerAvailable = true;

  void* StackPointer() const noexcept {
    return rsp_;
  }
};

}  // namespace sure
