#include <twist/rt/thr/random/number.hpp>

#include <random>

namespace twist::rt::thr {

namespace random {

// Use thread-local pseudo-random uniform generators
// initialized with random seeds

//////////////////////////////////////////////////////////////////////

struct RandomUInt64Generator {
  using Value = uint64_t;

 public:
  RandomUInt64Generator()
      : twister_(ChooseSeed()) {
  }

  Value Next() {
    return twister_();
  }

 private:
  static std::random_device::result_type ChooseSeed() {
    return std::random_device{}();
  }

 private:
  std::mt19937_64 twister_;
};

//////////////////////////////////////////////////////////////////////

uint64_t UInt64() {
  static thread_local RandomUInt64Generator generator;

  return generator.Next();
}

}  // namespace random

}  // namespace twist::rt::thr
