#include <twist/rt/thr/fault/std_like/thread.hpp>

#include <twist/rt/thr/fault/affinity/affinity.hpp>

#include <twist/rt/thr/fault/adversary/adversary.hpp>

namespace twist::rt::thr {
namespace fault {

void FaultyThread::Enter() {
  test::SetThreadAffinity();
  Adversary()->Enter();
}

void FaultyThread::Exit() {
  Adversary()->Exit();
}

}  // namespace fault
}  // namespace twist::thread
