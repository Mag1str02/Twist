#include "wait_queue.hpp"

#include "../simulator.hpp"

namespace twist::rt::sim {

namespace system {

void WaitQueue::AllocateSchedulerQueue() {
  waiters_ = Simulator::Current()->Scheduler()->NewWaitQueue();
}

}  // namespace system

}  // namespace twist::rt::sim
