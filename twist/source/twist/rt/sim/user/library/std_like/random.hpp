#pragma once

#include <twist/rt/sim/user/syscall/random.hpp>

#include <limits>
#include <random>

namespace twist::rt::sim {

namespace user::library::std_like {

class random_device {
 public:
  using result_type = unsigned int;

  result_type operator()() {
    return Next();
  }

  // Characteristics

  double entropy() const noexcept {
    return 32;
  }

  static constexpr result_type min() noexcept {
    return 0;
  }

  static constexpr result_type max() noexcept {
    return std::numeric_limits<result_type>::max();
  }

 private:
  result_type Next() {
    return syscall::RandomNumber();
  }
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
