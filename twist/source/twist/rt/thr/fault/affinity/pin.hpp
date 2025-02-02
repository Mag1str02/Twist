#pragma once

namespace twist::rt {
namespace thr {

// Pin current thread to a single cpu core

void PinThisThread(int desired_cpu);

}  // namespace thr
}  // namespace twist::rt
