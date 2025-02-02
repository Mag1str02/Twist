#pragma once

#include <twist/rt/thr/fault/adversary/adversary.hpp>
#include <twist/rt/thr/fault/adversary/yielder.hpp>

#include <fmt/core.h>

namespace twist::rt::thr {
namespace fault {

/////////////////////////////////////////////////////////////////////

class YieldAdversary : public IAdversary {
 public:
  YieldAdversary(size_t yield_freq) : yielder_(yield_freq) {
  }

  // Per-test methods

  void Reset() override {
    yielder_.Reset();
  }

  void Iter(size_t /*index*/) override {
  }

  void PrintReport() override {
    ::fmt::println("Context switches injected: {}", yielder_.YieldCount());
  }

  // Per-thread methods

  void Enter() override {
    // Do nothing
  }

  void Fault(FaultPlacement) override {
    yielder_.MaybeYield();
  }

  void ReportProgress() override {
    // Ignore lock-free algorithms
  }

  void Exit() override {
    // Do nothing
  }

 private:
  Yielder yielder_;
};

}  // namespace fault
}  // namespace twist::rt::thr
