#include "checkpoint.hpp"
#include "new_iter.hpp"

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user::test {

void Checkpoint() {
  rt::sim::system::Simulator::Current()->Checkpoint();
}

void NewIterHint() {
  rt::sim::system::Simulator::Current()->SysSchedHintNewIter();
}

}  // namespace user::test

}  // namespace twist::rt::sim
