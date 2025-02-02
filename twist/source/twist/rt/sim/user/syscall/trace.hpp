#pragma once

#include <twist/trace/event.hpp>

namespace twist::rt::sim {

namespace user::syscall {

trace::Id NewScopeId();

void Trace(trace::Event*);

}  // namespace user::syscall

}  // namespace twist::rt::sim
