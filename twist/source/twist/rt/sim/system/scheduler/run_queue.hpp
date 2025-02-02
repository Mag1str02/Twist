#pragma once

#include "../fwd.hpp"

namespace twist::rt::sim {

namespace system {

struct IRunQueue {
  virtual ~IRunQueue() = default;

  virtual bool IsIdle() const = 0;
  virtual Thread* PickNext() = 0;

  // Burn
  virtual void Remove(Thread*) = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
