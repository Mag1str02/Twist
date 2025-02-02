#pragma once

#include "event.hpp"

namespace twist::rt::sim {

namespace system {

namespace log {

struct ILogger {
  virtual void Add(Event*) = 0;
};

}  // namespace log

}  // namespace system

}  // namespace twist::rt::sim
