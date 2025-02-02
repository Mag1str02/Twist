#pragma once

namespace twist::rt::thr {

template <typename T>
class StaticVar {
 public:
  StaticVar() {
  }

  T& Ref() {
    return var_;
  }

  T* Ptr() {
    return &Ref();
  }

  T& operator*() {
    return Ref();
  }

  T* operator->() {
    return Ptr();
  }

  T* operator&() {
    return Ptr();
  }

 private:
  T var_;
};

}  // namespace twist::rt::thr
