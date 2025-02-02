#pragma once

#include <sure/trampoline.hpp>
#include <sure/stack_view.hpp>

#include <sure/machine/context.hpp>
#include <sure/sanitizer/context.hpp>
#include <sure/exceptions/context.hpp>

#include <sure/build.hpp>

namespace sure {

// Execution Context =
// 1) Machine context (registers) +
// 2) [Address | Thread] sanitizer context +
// 3) Exceptions context

class ExecutionContext final : private ITrampoline {
 public:
  // Empty context, cannot be a target for SwitchTo
  ExecutionContext();

  // Non-copyable
  ExecutionContext(const ExecutionContext&) = delete;
  ExecutionContext& operator=(const ExecutionContext&) = delete;

  // Non-movable
  ExecutionContext(ExecutionContext&&) = delete;
  ExecutionContext& operator=(ExecutionContext&&) = delete;

  // Prepare execution context for running trampoline->Run()
  // on stack `stack`
  void Setup(StackView stack, ITrampoline* trampoline);

  // Symmetric Control Transfer
  // 1) Save current execution context to `this`
  // 2) Activate `target` context
  void SwitchTo(ExecutionContext& target);

  // Leave current execution context forever
  // Last context switch in ITrampoline::Run
  [[noreturn]] void ExitTo(ExecutionContext& target);

  // For overflow checking
  // Calling StackPointer on non-suspended ExecutionContext results in UB
  void* StackPointer() const noexcept {
    return machine_.StackPointer();
  }

 private:
  // ITrampoline
  void Run() noexcept override;

  // Finalize first context switch
  void AfterStart();

 private:
  ITrampoline* user_trampoline_;

  MachineContext machine_;
  SanitizerContext sanitizer_;
  ExceptionsContext exceptions_;
};

}  // namespace sure
