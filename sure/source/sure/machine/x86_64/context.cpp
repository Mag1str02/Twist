#include <sure/machine/x86_64/context.hpp>

namespace sure {

// https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/
static void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void* arg7) {
  ITrampoline* t = (ITrampoline*)arg7;
  t->Run();
}

void MachineContext::Setup(StackView stack, ITrampoline* trampoline) {
  rsp_ = SetupMachineContext((void*)&stack.back(), (void*)MachineContextTrampoline, (void*)trampoline);
}

}  // namespace sure
