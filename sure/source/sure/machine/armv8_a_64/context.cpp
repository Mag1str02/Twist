#include <sure/machine/armv8_a_64/context.hpp>

namespace sure {

static void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void*, void*, void* arg9) {
  ITrampoline* t = (ITrampoline*)arg9;
  t->Run();
}

void MachineContext::Setup(StackView stack, ITrampoline* trampoline) {
  rsp_ = SetupMachineContext((void*)&stack.back(), (void*)MachineContextTrampoline, (void*)trampoline);
}

void MachineContext::SwitchTo(MachineContext& target) {
  SwitchMachineContext(&rsp_, &target.rsp_);
}

}  // namespace sure
