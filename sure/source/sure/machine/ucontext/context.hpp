#pragma once

#include <sure/trampoline.hpp>
#include <sure/stack_view.hpp>

#include <ucontext.h>

namespace sure {

struct MachineContext {
  ucontext_t context;

  MachineContext();

  void Setup(StackView stack, ITrampoline* trampoline);
  void SwitchTo(MachineContext& target);

  static constexpr bool kStackPointerAvailable = true;

  void* StackPointer() const noexcept;
};

}  // namespace sure
