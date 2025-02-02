#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/step_over.hpp>

namespace twist::rt::cap {

namespace assist {

//

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/assist/step_over.hpp>

namespace twist::rt::cap {

namespace assist {

//

}  // namespace assist

}  // namespace twist::rt::cap

#endif
