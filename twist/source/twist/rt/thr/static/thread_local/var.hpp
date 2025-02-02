#pragma once

#include <cassert>
#include <utility>

namespace twist::rt::thr {

template <typename T>
class StaticThreadLocalVar {
 public:
  template <typename ... Args>
  StaticThreadLocalVar(Args&& ... args)
      : var_(std::forward<Args>(args)...) {
    static_assert(!std::is_pointer_v<T>,
        "Consider TWISTED_STATIC_THREAD_LOCAL_PTR macro for static thread_local pointer declaration");
  }

  T& operator*() {
    return var_;
  }

  T* operator->() {
    return &var_;
  }

  void operator=(T v) {
    var_ = std::move(v);
  }

  T* operator&() {
    return &var_;
  }

 private:
  T var_;
};

}  // namespace twist::rt::thr
