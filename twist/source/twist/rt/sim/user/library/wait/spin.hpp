#pragma once

#include <twist/rt/sim/user/scheduler/model.hpp>
#include <twist/rt/sim/user/syscall/yield.hpp>
#include <twist/rt/sim/user/syscall/abort.hpp>

#include <twist/build.hpp>

namespace twist::rt::sim {

namespace user {

class [[nodiscard]] SpinWait {
 public:
  SpinWait() = default;

  // Non-copyable
  SpinWait(const SpinWait&) = delete;
  SpinWait& operator=(const SpinWait&) = delete;

  // Non-movable
  SpinWait(SpinWait&&) = delete;
  SpinWait& operator=(SpinWait&&) = delete;

  void Spin(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    Yield(call_site);
  }

  void operator()(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    Spin(call_site);
  }

  [[nodiscard]] bool ConsiderParking() const;

  [[nodiscard]] bool KeepSpinning() const {
    return !ConsiderParking();
  }

 private:
  void Yield(wheels::SourceLocation call_site) {
    ++yield_count_;
    system::UserContext ctx{"SpinWait::Spin()", call_site};
    syscall::Yield(&ctx);
  }

 private:
  size_t yield_count_ = 0;
};

inline void CpuRelax(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  system::UserContext ctx{"twist::ed::CpuRelax()", call_site};
  syscall::Yield(&ctx);
}

}  // namespace user

}  // namespace twist::rt::sim
