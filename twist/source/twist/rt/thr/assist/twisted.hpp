#pragma once

#include <utility>

namespace twist::rt::thr {

namespace assist {

template <typename T, bool Twist>
class Twisted {
 public:
  template <typename ... Args>
  Twisted(Args&& ... args)
      : object_(std::forward<Args>(args)...) {
  }

  struct Accessor {
    T* operator->() {
      return object_;
    }

   private:
    T* object_;
  };

  Accessor* operator->() {
    return {object_};
  }

 private:
  T object_;
};

}  // namespace assist

}  // namespace twist::rt::thr
