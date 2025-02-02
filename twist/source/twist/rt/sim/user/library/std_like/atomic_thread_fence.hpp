#pragma once

#include <twist/rt/sim/user/syscall/sync.hpp>

#include <wheels/core/source_location.hpp>

// std::memory_order
#include <atomic>

namespace twist::rt::sim {

namespace user::library::std_like {

// NOLINTNEXTLINE
inline void atomic_thread_fence(std::memory_order mo, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
  system::sync::Action fence{nullptr, 0, system::sync::ActionType::AtomicThreadFence, 0 /*ignored*/, mo,
                             system::sync::ValueType::None, "atomic_thread_fence", call_site};
  syscall::Sync(&fence);
}

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
