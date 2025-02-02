#pragma once

#include <wheels/core/defer.hpp>

#include <thread>
#include <tuple>

namespace twist::rt::thr {
namespace fault {

//////////////////////////////////////////////////////////////////////

class FaultyThread {
 private:
  using Impl = ::std::thread;

 public:
  // Standard member classes
  using id = Impl::id;
  using native_handle_type = Impl::native_handle_type;

 public:
  FaultyThread() = default;

  template <typename F, typename ... Args>
  FaultyThread(F&& fn, Args&&... args)
      : impl_([fn = std::move(fn), args = std::make_tuple(std::forward<Args>(args)...), this]() mutable {
          Enter();

          wheels::Defer defer{[this] {
            Exit();
          }};

          std::apply(fn, std::move(args));
        }) {
  }

  // Non-copyable
  FaultyThread(const FaultyThread&) = delete;
  FaultyThread& operator=(const FaultyThread&) = delete;

  // Move-constructible
  FaultyThread(FaultyThread&& that)
      : impl_(std::move(that.impl_)) {
  }

  // Move-assignable
  FaultyThread& operator=(FaultyThread&& that) {
    impl_ = std::move(that.impl_);
    return *this;
  }

  // NOLINTNEXTLINE
  bool joinable() const {
    return impl_.joinable();
  }

  // NOLINTNEXTLINE
  void join() {
    impl_.join();
  }

  // NOLINTNEXTLINE
  void detach() {
    impl_.detach();
  }

  void swap(FaultyThread& that) {
    impl_.swap(that.impl_);
  }

  ::std::thread::id get_id() const noexcept {
    return impl_.get_id();
  }

  // NOLINTNEXTLINE
  static unsigned int hardware_concurrency() noexcept {
    return ::std::thread::hardware_concurrency();
  }

  // NOLINTNEXTLINE
  native_handle_type native_handle() {
    return impl_.native_handle();
  }

 private:
  void Enter();
  void Exit();

 private:
  ::std::thread impl_;
};

//////////////////////////////////////////////////////////////////////

namespace this_thread {

// No fault injection needed
using ::std::this_thread::get_id;
using ::std::this_thread::yield;
using ::std::this_thread::sleep_for;
using ::std::this_thread::sleep_until;

}  // namespace this_thread

}  // namespace fault
}  // namespace twist::rt::thr
