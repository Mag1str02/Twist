#pragma once

#include <sure/stack_view.hpp>

namespace sure {

struct SanitizerContext {
  void Setup(StackView) {
    // Nop
  }

  void AfterStart() {
    // Nop
  }

  void BeforeSwitch(SanitizerContext& /*target*/) {
    // Nop
  }

  void AfterSwitch() {
    // Nop
  }

  void BeforeExit(SanitizerContext& /*target*/) {
    // Nop
  }
};

}  // namespace sure
