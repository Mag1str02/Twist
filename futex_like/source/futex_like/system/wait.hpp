#pragma once

#include <cstdint>

namespace futex_like {

namespace system {

int Wait(uint32_t* loc, uint32_t old);
int WaitTimed(uint32_t* loc, uint32_t old, uint32_t millis);

int WakeOne(uint32_t* loc);
int WakeAll(uint32_t* loc);

}  // namespace system

}  // namespace futex_like
