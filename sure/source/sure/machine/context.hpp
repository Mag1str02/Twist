#pragma once

#if defined(SURE_MACHINE_CONTEXT_x86_64)

#include <sure/machine/x86_64/context.hpp>

#elif defined(SURE_MACHINE_CONTEXT_armv8_a_64)

#include <sure/machine/armv8_a_64/context.hpp>

#elif defined(SURE_MACHINE_CONTEXT_ucontext)

#include <sure/machine/ucontext/context.hpp>

#else

#include <sure/machine/no/context.hpp>

#endif
