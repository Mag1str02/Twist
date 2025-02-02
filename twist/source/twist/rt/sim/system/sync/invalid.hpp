#pragma once

#include "atomic.hpp"
#include "non_atomic.hpp"

namespace twist::rt::sim {

namespace system::sync {

void DeadAtomicVarAccess(AtomicVar*, Action*);
void UnknownAtomicVarAccess(Action*);
void UnknownAtomicVarAccess(void*);

void DeadSharedVarAccess(Action*);
void UnknownSharedVarAccess(Access*);
void InitOnLiveSharedVar(Access*);

}  // namespace system::sync

}  // namespace twist::rt::sim
