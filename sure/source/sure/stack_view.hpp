#pragma once

// std::byte
#include <cstddef>

#include <span>

namespace sure {

using StackView = std::span<std::byte>;

}  // namespace sure
