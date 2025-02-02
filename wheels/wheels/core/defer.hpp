#pragma once

#include <functional>

namespace wheels {

// Calls a function when this object goes out of scope
// Inspired by a 'defer' statement from Go programming language:
// https://tour.golang.org/flowcontrol/12

// Usage: Defer defer( [this]() { CleanUp(); } );

template <typename F>
class Defer {
 public:
  Defer(F&& f) : func_(std::forward<F>(f)) {
  }

  ~Defer() {
    func_();
  }

 private:
  F func_;
};

}  // namespace wheels
