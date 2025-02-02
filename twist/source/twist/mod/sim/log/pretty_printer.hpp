#pragma once

#include "logger.hpp"

#include <ostream>

namespace twist::sim {

namespace log {

class PrettyPrinter : public ILogger {
 public:
  PrettyPrinter(std::ostream& out);
  PrettyPrinter();  // std::cout

  void Add(Event*) override;

 private:
  std::ostream& out_;
};

}  // namespace log

}  // namespace twist::sim
