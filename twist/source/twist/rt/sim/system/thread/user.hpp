#pragma once

#include <wheels/core/source_location.hpp>

namespace twist::rt::sim {

namespace system {

struct IThreadRoutine {
  virtual ~IThreadRoutine() = default;

  virtual void RunUser() = 0;
};

struct IThreadObject {
  virtual ~IThreadObject() = default;

  virtual wheels::SourceLocation SourceLoc() const noexcept = 0;
};

struct IThreadExitHandler {
  virtual ~IThreadExitHandler() = default;

  virtual void AtThreadExit() noexcept = 0;
};

struct IThreadUserState : IThreadRoutine,
                          IThreadObject,
                          IThreadExitHandler {
};

}  // namespace system

}  // namespace twist::rt::sim
