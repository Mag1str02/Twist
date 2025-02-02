#pragma once

#include "../syscall/abort.hpp"
#include "../syscall/write.hpp"

#include <twist/build.hpp>

#include <string_view>

namespace twist::rt::sim {

namespace user::assist {

void Prune(std::string_view why) {
  if constexpr (twist::build::IsolatedSim()) {
    syscall::Write(2, why);
    syscall::Abort(system::Status::Pruned);
  }
}

}  // namespace user::assist

}  // namespace twist::rt::sim
