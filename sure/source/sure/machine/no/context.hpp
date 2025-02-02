#pragma once

#include <sure/trampoline.hpp>
#include <sure/stack_view.hpp>

namespace sure {

struct MachineContext {

  MachineContext();

  void Setup(StackView stack, ITrampoline* trampoline);
  void SwitchTo(MachineContext& target);

  static constexpr bool kStackPointerAvailable = false;

  void* StackPointer() const noexcept;
};

}  // namespace sure
