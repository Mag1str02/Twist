#pragma once

#include <twist/rt/thr/fault/adversary/adversary.hpp>

namespace twist::rt::thr {
namespace fault {

IAdversaryPtr CreateLockFreeAdversary();

}  // namespace fault
}  // namespace twist::rt::thr
