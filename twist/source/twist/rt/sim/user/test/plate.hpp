#pragma once

#include <twist/rt/sim/user/assist/shared.hpp>
#include <twist/rt/sim/user/syscall/id.hpp>

#include <wheels/core/source_location.hpp>

#include <cstddef>

namespace twist::rt::sim {

namespace user::test {

class Plate {
 public:
  explicit Plate(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : last_owner_(system::kImpossibleThreadId, source_loc) {
  }

  void Access(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    last_owner_.Write(syscall::GetId(), call_site);
    ++count_;
  }

  size_t AccessCount() const {
    return count_;
  }

 private:
  user::assist::Shared<system::ThreadId> last_owner_;
  size_t count_ = 0;
};

}  // namespace user::test

}  // namespace twist::rt::sim
