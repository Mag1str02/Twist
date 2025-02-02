#pragma once

namespace sure {

struct ExceptionsContext {
  void SwitchTo(ExceptionsContext& /*target*/) {
    // Nop
  }
};

}  // namespace sure
