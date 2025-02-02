#include "inject_fault.hpp"

#if defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/adversary/inject_fault.hpp>

namespace twist::rt::thr {
namespace test {

void InjectFault(wheels::SourceLocation) {
  rt::thr::fault::InjectFault();
}

}  // namespace test
}  // namespace twist::test

#else

namespace twist::rt::thr {
namespace test {

void InjectFault(wheels::SourceLocation) {
  // No-op
}

}  // namespace test
}  // namespace twist::test

#endif
