#pragma once

#include <stdint.h>

int __ulock_wait(uint32_t operation, void* addr, uint64_t value,
                 uint32_t timeout); /* timeout is specified in microseconds */

int __ulock_wake(uint32_t operation, void* addr, uint64_t wake_value);

#define UL_COMPARE_AND_WAIT 1
#define ULF_WAKE_ALL 0x00000100
