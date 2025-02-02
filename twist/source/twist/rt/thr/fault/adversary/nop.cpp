#include <twist/rt/thr/fault/adversary/nop.hpp>

namespace twist::rt::thr {
namespace fault {

/////////////////////////////////////////////////////////////////////

class NopAdversary : public IAdversary {
 public:
  void Reset() override {
  }

  void Iter(size_t /*index*/) override {
  }

  void PrintReport() override {
  }

  // Per-thread methods

  void Enter() override {
    // Do nothing
  }

  void Fault(FaultPlacement) override {
    // Do nothing
  }

  void ReportProgress() override {
    // Ignore lock-free algorithms
  }

  void Exit() override {
    // Do nothing
  }
};

/////////////////////////////////////////////////////////////////////

IAdversaryPtr CreateNopAdversary() {
  return std::make_shared<NopAdversary>();
}

}  // namespace fault
}  // namespace twist::rt::thr
