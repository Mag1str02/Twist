#pragma once

namespace twist::rt::sim {

namespace user::scheduler {

// compare_exchange_weak, try_lock
bool SpuriousTryFailure();

}  // namespace user::scheduler

}  // namespace twist::rt::sim
