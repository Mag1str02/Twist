#pragma once

#include <cstddef>
#include <span>

namespace twist::rt::thr {

namespace fiber {

using StackView = std::span<std::byte>;

}  // namespace fiber

}  // namespace twist::rt::thr
