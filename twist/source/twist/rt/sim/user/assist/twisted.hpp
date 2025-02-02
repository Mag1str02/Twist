#pragma once

#include <twist/rt/sim/user/scheduler/preemption.hpp>

#include <utility>
#include <optional>

namespace twist::rt::sim {

namespace user::assist {

template <typename T, bool Twist>
class Twisted {
  using Guard = user::scheduler::PreemptionGuard;
 public:
  template <typename ... Args>
  Twisted(Args&& ... args) {
    Guard g(Twist);
    object_.emplace(std::forward<Args>(args)...);
  }

  ~Twisted() {
    Guard g{Twist};
    object_.reset();
  }

  struct Accessor : Guard {
    Accessor(T& object)
        : Guard{Twist},
          object_(&object) {
      //
    }

    Accessor(const Accessor&) = delete;
    Accessor(Accessor&&) = delete;

    T* operator->() const {
      // user::scheduler::Interrupt();
      return object_;
    }

   private:
    T* object_;
  };

  Accessor operator->() {
    return {*object_};
  }

 private:
  // TODO: manual lifetime
  std::optional<T> object_;
};

}  // namespace user::assist

}  // namespace twist::rt::sim
