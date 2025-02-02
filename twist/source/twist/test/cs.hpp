#pragma once

#include <twist/test/plate.hpp>

namespace twist::test {

class CriticalSection {
 public:
  void operator()() {
    shared_plate_.Access();
  }

 private:
  Plate shared_plate_;
};

}  // namespace twist::test
