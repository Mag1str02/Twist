#pragma once

#include <twist/rt/thr/fault/adversary/fault_placement.hpp>

#include <memory>

namespace twist::rt::thr {
namespace fault {

/////////////////////////////////////////////////////////////////////

class IAdversary {
 public:
  virtual ~IAdversary() = default;

  // Test

  virtual void Reset() = 0;
  virtual void Iter(size_t index) = 0;

  virtual void PrintReport() = 0;

  // This thread

  virtual void Enter() = 0;

  // Inject fault (yield, sleep, park)
  virtual void Fault(FaultPlacement) = 0;

  // For lock-free and wait-free algorithms
  virtual void EnablePark() {}
  virtual void DisablePark() {};
  virtual void ReportProgress() = 0;

  virtual void Exit() = 0;
};

using IAdversaryPtr = std::shared_ptr<IAdversary>;

/////////////////////////////////////////////////////////////////////

// Not thread safe, should be externally synchronized

IAdversary* Adversary();

IAdversaryPtr GetAdversary();
void SetAdversary(IAdversaryPtr adversary);

void AccessAdversary();

}  // namespace fault
}  // namespace twist::rt::thr
