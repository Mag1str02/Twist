#pragma once

#include "status.hpp"

#include <string>

namespace twist::rt::sim {

namespace system {

void AbortIter(Status, std::string err);

}  // namespace system

}  // namespace twist::rt::sim
