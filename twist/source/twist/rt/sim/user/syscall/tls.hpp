#pragma once

#include <twist/rt/sim/user/static/thread_local/storage.hpp>

namespace twist::rt::sim {

namespace user::syscall {

user::tls::Storage& AccessTls();

}  // namespace user::syscall

}  // namespace twist::rt::sim
