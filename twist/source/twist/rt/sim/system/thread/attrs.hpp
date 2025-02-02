#pragma once

namespace twist::rt::sim {

struct ThreadAttrs {
  bool preemptive;
  bool lock_free;
};

}  // namespace twist::rt::sim
