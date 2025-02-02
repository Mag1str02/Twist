#include "domain.hpp"

namespace twist::trace {

Domain* Domain::Global() {
  static Domain global{"Global"};
  return &global;
}

}  // namespace twist::trace
