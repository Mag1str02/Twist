#pragma once

#include <cstdint>
#include <cstdlib>

namespace twist::rt::sim {

namespace user::syscall {

uint64_t RandomNumber();

size_t RandomChoice(size_t alts);

}  // namespace user::syscall

}  // namespace twist::rt::sim
