#pragma once

#include <cstddef>

namespace twist::rt::sim {

namespace user::scheduler {

// Returns previous status
bool SetThreadAttrLockFree(bool status);

void ReportProgress();

}  // namespace scheduler

}  // namespace twist::rt::sim
