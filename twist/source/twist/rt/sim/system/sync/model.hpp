#pragma once

#include "atomic.hpp"
#include "non_atomic.hpp"
#include "alloc.hpp"

#include "../thread/struct.hpp"
#include "../thread/count.hpp"

#include "invalid.hpp"

#include <map>

namespace twist::rt::sim {

namespace system::sync {

// A Promising Semantics for Relaxed-Memory Concurrency
// https://sf.snu.ac.kr/publications/promising.pdf

class SeqCstMemoryModel {
 public:
  SeqCstMemoryModel() {
  }

  void Reset() {
    atomics_.clear();
    non_atomics_.clear();
    allocs_.clear();
  }

  // Stat

  size_t AtomicCount() const {
    return atomics_.size();
  }

  size_t NonAtomicCount() const {
    return non_atomics_.size();
  }

  // Threads

  void Tick(Thread* thread) {
    thread->sync.clock.current.Tick(thread->id);
    thread->sync.clock.epoch += 1;
  }

  void Spawn(Thread* parent, Thread* child) {
    child->sync.clock.current.Assign(parent->sync.clock.current);
    Tick(child);
  }

  void Wake(Thread* waker, Thread* waiter) {
    // Wake acts as release operation
    waiter->sync.clock.current.Join(waker->sync.clock.release);
    Tick(waiter);
  }

  // Sync

  uint64_t Sync(Thread* thread, Action* action) {
    Tick(thread);

    switch (action->type) {
      case ActionType::AtomicInit:
        AtomicInit(thread, action);
        return 0;
      case ActionType::AtomicDestroy:
        AtomicDestroy(thread, action);
        return 0;
      case ActionType::AtomicLoad:
        return AtomicLoad(thread, action);
      case ActionType::AtomicDebugLoad:
        return AtomicDebugLoad(thread, action);
      case ActionType::AtomicStore:
        AtomicStore(thread, action);
        return 0;
      case ActionType::AtomicRmwLoad:
        return AtomicRmwLoad(thread, action);
      case ActionType::AtomicRmwCommit:
        AtomicRmwCommit(thread, action);
        return 0;
      case ActionType::AtomicThreadFence:
        AtomicThreadFence(thread, action);
        return 0;
      default:
        WHEELS_PANIC("Unhandled synchronization action");
    }
  }

  // Futex

  uint64_t FutexLoad(Thread* /*thread*/, AtomicVar* atomic) {
    return atomic->last_store.value;
  }

  AtomicVar* TryFutexAtomic(void* loc) {
    return TryGetShadowAtomicVar(loc);
  }

  AtomicVar* FutexAtomic(void* loc) {
    AtomicVar* atomic = TryGetShadowAtomicVar(loc);

    if ((atomic == nullptr) || atomic->dtor) {
      UnknownAtomicVarAccess(loc);
      WHEELS_UNREACHABLE();
    }

    return atomic;
  }

  // Non-atomics

  std::optional<OldAccess> AccessNonAtomicVar(Thread* thread, Access* access) {
    Tick(thread);

    if (access->type == AccessType::Init) {
      return InitNonAtomicVar(thread, access);
    } else {
      return AccessLiveNonAtomicVar(thread, access);
    }
  }

  // Memory allocations

  void Malloc(Thread* thread, void* addr, size_t size) {
    Alloc* alloc = GetShadowAllocation(addr);
    {
      alloc->alive = true;
      alloc->size = size;
      alloc->thread_id = thread->id;

      auto* scope = thread->fiber->scope;
      if ((scope != nullptr) && !IsRuntime(scope)) {
        alloc->source_loc = scope->GetLoc();
      } else {
        alloc->source_loc.reset();
      }
    }
    thread->sync.clock.current.Join(alloc->free);
  }

  void Free(Thread* thread, void* addr) {
    Alloc* alloc = GetShadowAllocation(addr);
    alloc->alive = false;
    alloc->free.Assign(thread->sync.clock.current);
  }

  const std::map<void*, Alloc>& Allocations() const {
    return allocs_;
  }

 private:
  // Atomics

  AtomicVar* TryGetShadowAtomicVar(void* loc) {
    auto it = atomics_.find(loc);
    if (it != atomics_.end()) {
      return &it->second;
    } else {
      return nullptr;
    }
  }

  AtomicVar* GetShadowAtomicVar(Action* action) {
    AtomicVar* atomic = TryGetShadowAtomicVar(action->loc);

    if (atomic == nullptr) {
      UnknownAtomicVarAccess(action);
      WHEELS_UNREACHABLE();
    }

    if (atomic->dtor) {
      DeadAtomicVarAccess(atomic, action);;
      WHEELS_UNREACHABLE();
    }

    return atomic;
  }

  void AtomicInit(Thread*, Action* init) {
    auto [it, ok] = atomics_.try_emplace(init->loc);
    AtomicVar* atomic = &it->second;

    atomic->source_loc = init->source_loc;
    atomic->size = init->size;
    atomic->last_store.value = init->value;
    atomic->last_store.clock.Init();  // Initialization is not atomic
    atomic->dtor.reset();
  }

  uint64_t AtomicLoad(Thread* reader, Action* load) {
    WHEELS_ASSERT(IsLoadOrder(load->mo), "Unexpected memory order for atomic load");

    AtomicVar* atomic = GetShadowAtomicVar(load);

    // Acquire load ~ Relaxed load ; Acquire fence
    uint64_t value = AtomicLoadRelaxed(reader, atomic);
    if (IsAcquireOrder(load->mo)) {
      AtomicThreadFenceAcquire(reader);
    }

    return value;
  }

  uint64_t AtomicDebugLoad(Thread* /*reader*/, Action* load) {
    AtomicVar* atomic = GetShadowAtomicVar(load);
    return atomic->last_store.value;
  }

  void AtomicStore(Thread* writer, Action* store) {
    WHEELS_ASSERT(IsStoreOrder(store->mo), "Unexpected memory order for atomic store");

    AtomicVar* atomic = GetShadowAtomicVar(store);

    // Atomic store ~ Release fence ; Relaxed store
    if (IsReleaseOrder(store->mo)) {
      AtomicThreadFenceRelease(writer);
    }
    AtomicStoreRelaxed(writer, atomic, store->value);
  }

  uint64_t AtomicRmwLoad(Thread* writer, Action* rmw) {
    AtomicVar* atomic = GetShadowAtomicVar(rmw);

    uint64_t value = AtomicLoadRelaxed(writer, atomic);
    if (IsAcquireOrder(rmw->mo)) {
      AtomicThreadFenceAcquire(writer);
    }
    return value;
  }

  void AtomicRmwCommit(Thread* writer, Action* rmw) {
    AtomicVar* atomic = GetShadowAtomicVar(rmw);

    if (IsReleaseOrder(rmw->mo)) {
      AtomicThreadFenceRelease(writer);
    }
    AtomicRmwCommitRelaxed(writer, atomic, rmw->value);
  }

  void AtomicDestroy(Thread*, Action* destroy) {
    AtomicVar* atomic = GetShadowAtomicVar(destroy);

    // Fix missing call site
    destroy->source_loc = atomic->source_loc;

    atomic->dtor.emplace();

    // atomics_.erase(destroy->loc);
  }

  static uint64_t AtomicLoadRelaxed(Thread* reader, AtomicVar* atomic) {
    reader->sync.clock.acquire.Join(atomic->last_store.clock);
    return atomic->last_store.value;
  }

  static void AtomicStoreRelaxed(Thread* writer, AtomicVar* atomic, uint64_t value) {
    atomic->last_store.value = value;
    atomic->last_store.clock.Assign(writer->sync.clock.release);
  }

  static void AtomicRmwCommitRelaxed(Thread* writer, AtomicVar* atomic, uint64_t value) {
    atomic->last_store.clock.Join(writer->sync.clock.release);
    atomic->last_store.value = value;
  }

  // Fences

  static void AtomicThreadFenceRelease(Thread* thread) {
    // release < current
    thread->sync.clock.release.Assign(thread->sync.clock.current);
  }

  static void AtomicThreadFenceAcquire(Thread* thread) {
    thread->sync.clock.current.Join(thread->sync.clock.acquire);
  }

  static void AtomicThreadFenceAcqRel(Thread* thread) {
    AtomicThreadFenceAcquire(thread);
    AtomicThreadFenceRelease(thread);
  }

  static void AtomicThreadFence(Thread* thread, Action* fence) {
    switch (fence->mo) {
      case std::memory_order::acquire:
        AtomicThreadFenceAcquire(thread);
        return;
      case std::memory_order::release:
        AtomicThreadFenceRelease(thread);
        return;
      case std::memory_order::acq_rel:
        AtomicThreadFenceAcqRel(thread);
        return;
      default:
        WHEELS_PANIC("Fence not supported");
    }
  }

 private:
  // Non-atomics

  static bool Conflict(AccessType a, AccessType b) {
    return IsWrite(a) || IsWrite(b);
  }

  static bool HappensBefore(const OldAccess& old, const Thread* thread) {
    return thread->sync.clock.current.Get(old.thread) >= old.epoch;
  }

  std::optional<OldAccess> CheckDataRaces(const Thread* thread, NonAtomicVar* var, const Access& curr) {
    if (var->last_write && (var->last_write->thread != thread->id)) {
      // Compare current access with the last write (from different thread)
      OldAccess& last_write = *(var->last_write);
      if (!HappensBefore(last_write, thread)) {
        return last_write;
      }
    }

    if (IsWrite(curr.type)) {
      // Compare current write with the last read from each thread

      size_t n = ThreadCount();
      for (size_t i = 1; i <= n; ++i) {
        if ((i != thread->id) && (var->last_read[i])) {
          OldAccess& old = *(var->last_read[i]);
          if (!HappensBefore(old, thread)) {
            return old;
          }
        }
      }
    }

    return std::nullopt;
  }

  std::optional<OldAccess> InitNonAtomicVar(Thread* thread, Access* access) {
    NonAtomicVar* var;

    auto [it, ok] = non_atomics_.try_emplace(access->loc);
    var = &it->second;

    if (!ok) {
      // Reuse location

      if (!var->dtor) {
        InitOnLiveSharedVar(access);
        WHEELS_UNREACHABLE();
      }

      if (auto race = CheckDataRaces(thread, var, *access)) {
        return race;
      }
    }

    var->Init();
    var->source_loc = access->source_loc;

    {
      // Log ctor access

      OldAccess curr{access->loc, access->type, thread->id,
                     thread->sync.clock.epoch, access->source_loc};

      var->Log(curr);
    }

    return std::nullopt;
  }

  std::optional<OldAccess> AccessLiveNonAtomicVar(Thread* thread, Access* access) {
    NonAtomicVar* var;

    auto it = non_atomics_.find(access->loc);
    if (it != non_atomics_.end()) {
      var = &it->second;
    } else {
      UnknownSharedVarAccess(access);
      WHEELS_UNREACHABLE();
    }

    if (auto race = CheckDataRaces(thread, var, *access)) {
      return race;
    }

    OldAccess curr{
        access->loc,
        access->type,
        thread->id,
        thread->sync.clock.epoch,
        access->source_loc
    };

    if (access->type == AccessType::Destroy) {
      //non_atomics_.erase(access.loc);
      var->dtor = curr;

      // Fix missing call site
      access->source_loc = var->source_loc;
    }

    var->Log(curr);

    return std::nullopt;
  }

  // Allocations

  Alloc* GetShadowAllocation(void* addr) {
    auto [it, ok] = allocs_.try_emplace(addr);
    Alloc* alloc = &(it->second);
    if (ok) {
      alloc->free.Init();
    }
    return alloc;
  }

  // Helper, move to proper place
  bool IsRuntime(twist::trace::Scope* scope) {
    auto* attr = scope->GetAttrs();
    while (attr != nullptr) {
      if (std::strcmp(attr->GetName(), "twist.rt") == 0) {
        return true;
      }
      attr = attr->next;
    }
    return false;
  }

 private:
  std::map<void*, AtomicVar> atomics_;
  std::map<void*, NonAtomicVar> non_atomics_;
  std::map<void*, Alloc> allocs_;
};

}  // namespace system::sync

}  // namespace twist::rt::sim
