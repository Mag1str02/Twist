#pragma once

#include "clock.hpp"
#include "access.hpp"

#include "../limits.hpp"
#include "../thread/id.hpp"

#include <wheels/core/source_location.hpp>

#include <optional>

namespace twist::rt::sim {

namespace system::sync {

// Non-atomic var

using NonAtomicDtor = OldAccess;

struct NonAtomicVar {
  wheels::SourceLocation source_loc;

  std::optional<OldAccess> last_read[kMaxThreads + 1];
  std::optional<OldAccess> last_write;
  std::optional<NonAtomicDtor> dtor;

  void Init() {
    for (size_t i = 1; i <= kMaxThreads; ++i) {
      last_read[i].reset();
    }
    last_write.reset();
    dtor.reset();
  }

  void Log(OldAccess a) {
    if (IsRead(a.type)) {
      last_read[a.thread] = a;
    } else {
      last_write = a;
    }
  }
};

}  // namespace system::sync

}  // namespace twist::rt::sim