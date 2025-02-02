#pragma once

#include <sure/stack/mmap.hpp>
#include <sure/stack/new.hpp>

namespace sure {

#if WINDOWS
using Stack = NewStack;
#else
using Stack = MmapStack;
#endif

}  // namespace sure
