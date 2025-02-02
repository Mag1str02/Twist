#pragma once

#include "../fwd.hpp"

#include <function2/function2.hpp>

namespace twist::rt::sim {

namespace system::call {

using Handler = fu2::function_view<bool(Thread*)>;

}  // namespace system::call

}  // namespace twist::rt::sim
