#pragma once

#include "fiber.hpp"

namespace twist::rt::thr {

namespace fiber {

void ResetStack(StackView stack);

void NewFiber(Fiber* fiber);
void NewFiber(Fiber* fiber, StackView stack);

Fiber* SwitchToFiber(Fiber* target);

Fiber* CurrentFiber();

}  // namespace fiber

}  // namespace twist::rt::thr
