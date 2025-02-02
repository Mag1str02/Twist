#pragma once

#include "fiber.hpp"

namespace twist::rt::sim {

namespace user::fiber {

void ResetStack(StackView stack);

void NewFiber(Fiber*);
void NewFiber(Fiber*, StackView stack);

Fiber* SwitchToFiber(Fiber* target);
Fiber* CurrentFiber();

}  // namespace user::fiber

}  // namespace twist::rt::sim
