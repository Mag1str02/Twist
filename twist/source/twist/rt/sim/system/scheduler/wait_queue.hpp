#pragma once

#include "../fwd.hpp"

#include <memory>

namespace twist::rt::sim {

namespace system {

struct IWaitQueue {
  virtual ~IWaitQueue() = default;

  virtual bool IsEmpty() const = 0;
  virtual void Push(Thread*) = 0;
  virtual Thread* Pop() = 0;
  virtual Thread* PopAll() = 0;
  virtual bool Remove(Thread*) = 0;
};

using IWaitQueuePtr = std::unique_ptr<IWaitQueue>;

}  // namespace system

}  // namespace twist::rt::sim
