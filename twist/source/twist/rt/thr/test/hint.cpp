#include "new_iter.hpp"

#if defined(__TWIST_FAULTY__)

#include <twist/rt/thr/fault/adversary/adversary.hpp>

namespace twist::rt::thr {
namespace test {

void NewIterHint() {
  rt::thr::fault::Adversary()->Iter(42);  // TODO: Fix later
}

}  // namespace test
}  // namespace twist::test

#else

namespace twist::rt::thr {
namespace test {

void NewIterHint() {
  // No-op
}

}  // namespace test
}  // namespace twist::test

#endif
