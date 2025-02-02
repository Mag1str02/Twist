#pragma once

#include <twist/rt/cap/static/thread_local/ptr.hpp>

// static thread_local T* name = nullptr
// → TWISTED_STATIC_THREAD_LOCAL_PTR(T, name)

// static thread_local T* name = init
// → TWISTED_STATIC_THREAD_LOCAL_PTR_INIT(T, name, init)

// Usage: examples/thread_local/main.cpp
