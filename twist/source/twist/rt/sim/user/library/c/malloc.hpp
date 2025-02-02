#pragma once

#include <cstdlib>

namespace twist::rt::sim {

namespace user::library::c {

void* malloc(size_t size);
void free(void* ptr);

}  // namespace user::library::c

}  // namespace twist::rt::sim
