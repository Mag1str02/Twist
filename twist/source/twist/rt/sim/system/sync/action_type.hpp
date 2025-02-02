#pragma once

namespace twist::rt::sim {

namespace system::sync {

enum class ActionType {
  AtomicInit,
  AtomicLoad,
  AtomicDebugLoad,
  AtomicStore,
  AtomicRmwLoad,
  AtomicRmwCommit,
  AtomicDestroy,
  AtomicThreadFence,
};

inline bool IsLoad(ActionType action) {
  switch (action) {
    case ActionType::AtomicLoad:
    case ActionType::AtomicDebugLoad:
    case ActionType::AtomicRmwLoad:
      return true;
    default:
      return false;
  }
}

inline bool IsStore(ActionType action) {
  switch (action) {
    case ActionType::AtomicInit:
    case ActionType::AtomicStore:
    case ActionType::AtomicRmwCommit:
      return true;
    default:
      return false;
  }
}

inline bool IsRmwStep(ActionType action) {
  switch (action) {
    case ActionType::AtomicRmwLoad:
    case ActionType::AtomicRmwCommit:
      return true;
    default:
      return false;
  }
}

inline bool IsDebug(ActionType action) {
  return action == ActionType::AtomicDebugLoad;
}

}  // namespace system::sync

}  // namespace twist::rt::sim
