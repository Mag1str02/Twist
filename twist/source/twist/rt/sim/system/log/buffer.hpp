#pragma once

#include <wheels/memory/view.hpp>

namespace twist::rt::sim {

namespace system {

namespace log {

wheels::MutableMemView Buffer();

wheels::MutableMemView CommentBuffer();

}  // namespace log

}  // namespace system

}  // namespace twist::rt::sim
