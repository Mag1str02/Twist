#include "manager.hpp"

#include <twist/rt/sim/user/fmt/temp.hpp>
#include <twist/rt/sim/user/safety/panic.hpp>

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user {

namespace ss {

bool Manager::IsMainRunning() const {
  return system::Simulator::TryCurrent() != nullptr;
}

void Manager::AbortOnException(IVar* global) {
  user::Panic(system::Status::UnhandledException,
      user::fmt::FormatTemp(
          "Exception thrown from constructor of static global variable '{}'",
          global->Name()),
      global->SourceLoc());
}

void* Manager::AllocVar(size_t size) {
  return system::Simulator::Current()->UserAllocStatic(size);
}

void Manager::DeallocVars() {
  return system::Simulator::Current()->UserDeallocStatics();
}

}  // namespace ss

}  // namespace user

}  // namespace twist::rt::sim
