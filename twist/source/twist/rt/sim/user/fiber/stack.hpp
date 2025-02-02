#pragma once

#include <cstddef>
#include <span>

namespace twist::rt::sim {

namespace user::fiber {

using StackView = std::span<std::byte>;

}  // namespace user::fiber

}  // namespace twist::rt::sim
