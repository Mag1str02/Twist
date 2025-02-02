#include <sure/machine/x86_64_win/context.hpp>

namespace sure {

// https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170#calling-convention-defaults
static void MachineContextTrampoline(void*, void*, void*, void*, void* arg5) {
  ITrampoline* t = (ITrampoline*)arg5;
  t->Run();
}

void MachineContext::Setup(StackView stack, ITrampoline* trampoline) {
  rsp_ = SetupMachineContext(
      (void*)&stack.back(), (void*)MachineContextTrampoline, (void*)trampoline);
}

}  // namespace sure
