#pragma once

#include <twist/mod/thr/main.hpp>

namespace twist::thr {

// Run test routine in test environment
void Run(TestRoutine test);

// Standalone run
void Run(TestRoutine test);

}  // namespace twist::thr
