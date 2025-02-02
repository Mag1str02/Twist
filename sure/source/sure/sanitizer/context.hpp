#pragma once

#if __has_feature(address_sanitizer)

#include <sure/sanitizer/address/context.hpp>

#elif __has_feature(thread_sanitizer)

#include <sure/sanitizer/thread/context.hpp>

#else

#include <sure/sanitizer/nop/context.hpp>

#endif
