#include <twist/rt/thr/fault/adversary/adversary.hpp>

#include <twist/rt/thr/fault/adversary/yield.hpp>

#include <wheels/core/singleton.hpp>

#include <memory>
#include <utility>

namespace twist::rt::thr {
namespace fault {

/////////////////////////////////////////////////////////////////////

class Holder {
 public:
  Holder() : adversary_(CreateDefaultAdversary()) {
  }

  IAdversary* Get() {
    return adversary_.get();
  }

  IAdversaryPtr GetPtr() {
    return adversary_;
  }

  void Set(IAdversaryPtr adversary) {
    adversary_ = std::move(adversary);
  }

 private:
  static IAdversaryPtr CreateDefaultAdversary() {
    return std::make_shared<YieldAdversary>(10);
  }

 private:
  IAdversaryPtr adversary_;
};

IAdversary* Adversary() {
  return Singleton<Holder>()->Get();
}

IAdversaryPtr GetAdversary() {
  return Singleton<Holder>()->GetPtr();
}

void SetAdversary(IAdversaryPtr adversary) {
  Singleton<Holder>()->Set(std::move(adversary));
}

void AccessAdversary() {
  (void)Adversary();
}

}  // namespace fault
}  // namespace twist::rt::thr
