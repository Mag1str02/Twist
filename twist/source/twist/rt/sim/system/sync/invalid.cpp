#include "model.hpp"

#include "../abort.hpp"

#include <twist/wheels/fmt/source_location.hpp>

#include <wheels/core/compiler.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

namespace twist::rt::sim {

namespace system::sync {

static void InvalidAccess(std::string err) {
  AbortIter(Status::InvalidMemoryAccess, std::move(err));
  WHEELS_UNREACHABLE();
}

// Atomics

void DeadAtomicVarAccess(AtomicVar* atomic, Action* action) {
  auto err = fmt::format("Accessing destroyed atomic: addr = {}, source location = {}, operation = {}, access source location = {}",
                         fmt::ptr(action->loc), atomic->source_loc, action->operation, action->source_loc);
  InvalidAccess(std::move(err));
}

void UnknownAtomicVarAccess(Action* action) {
  auto err = fmt::format("Invalid atomic access: addr = {}, operation = {}, source location = {}",
                         fmt::ptr(action->loc), action->operation, action->source_loc);
  InvalidAccess(std::move(err));
}

void UnknownAtomicVarAccess(void* loc) {
  auto err = fmt::format("Invalid atomic access: addr = {}", fmt::ptr(loc));
  InvalidAccess(std::move(err));
}

// Shared

void DeadSharedVarAccess(NonAtomicVar* /*shared*/, Access* access) {
  auto err = fmt::format("Accessing destroyed shared variable: addr = {}, access source location = {}", fmt::ptr(access->loc), access->source_loc);
  InvalidAccess(std::move(err));
}

void UnknownSharedVarAccess(Access* access) {
  auto err = fmt::format("Invalid shared variable access: addr = {}, source location = {}", fmt::ptr(access->loc), access->source_loc);
  InvalidAccess(std::move(err));
}

void InitOnLiveSharedVar(Access* access) {
  auto err = fmt::format("Trying to initialize shared variable on memory location that is already occupied by another live shared variable: addr = {}, access source location = {}", fmt::ptr(access->loc), access->source_loc);
  InvalidAccess(std::move(err));
}

}  // namespace system::sync

}  // namespace twist::rt::sim
