#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/assist/fiber.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::sim::user::assist::StackView;
using rt::sim::user::assist::ResetFiberStack;

using rt::sim::user::assist::Fiber;
using rt::sim::user::assist::FiberHandle;

using rt::sim::user::assist::NewFiber;
using rt::sim::user::assist::SwitchToFiber;

}  // namespace assist

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/assist/fiber.hpp>

namespace twist::rt::cap {

namespace assist {

using rt::thr::assist::StackView;
using rt::thr::assist::ResetFiberStack;

using rt::thr::assist::Fiber;
using rt::thr::assist::FiberHandle;

using rt::thr::assist::NewFiber;
using rt::thr::assist::SwitchToFiber;

}  // namespace assist

}  // namespace twist::rt::cap

#endif
