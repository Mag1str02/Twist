#include "am_i_user.hpp"

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user {

bool AmIUser() {
  return system::Simulator::Current()->AmIUser();
}

}  // namespace user

}  // namespace twist::rt::sim
