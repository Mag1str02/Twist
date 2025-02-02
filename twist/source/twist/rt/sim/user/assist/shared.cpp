#include "shared.hpp"

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user::assist {

static void NonAtomicAccess(system::sync::Access access) {
  system::Simulator::Current()->SysAccessNonAtomicVar(&access);
}


void NonAtomicInit(void* loc, wheels::SourceLocation source_loc) {
  NonAtomicAccess({loc, system::sync::AccessType::Init, source_loc});
}

void NonAtomicRead(void* loc, wheels::SourceLocation source_loc) {
  NonAtomicAccess({loc, system::sync::AccessType::Read, source_loc});
}

void NonAtomicWrite(void* loc, wheels::SourceLocation source_loc) {
  NonAtomicAccess({loc, system::sync::AccessType::Write, source_loc});
}

void NonAtomicDestroy(void* loc, wheels::SourceLocation source_loc) {
  NonAtomicAccess({loc, system::sync::AccessType::Destroy, source_loc});
}

}  // namespace user::assist

}  // namespace twist::rt::sim
