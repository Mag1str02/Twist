#pragma once

namespace twist::rt::thr {

namespace assist {

template <typename T>
class SharedWriteView {
 public:
  SharedWriteView(T* v)
      : v_(v) {
  }

  T& operator*() noexcept {
    Write();
    return *v_;
  }

  T* operator->() noexcept {
    Write();
    return v_;
  }

 private:
  void Write() noexcept {
    // No-op
  }

 private:
  T* v_;
};

template <typename T>
class SharedReadView {
 public:
  SharedReadView(const T* v)
      : v_(v) {
  }

  const T& operator*() const noexcept {
    Read();
    return *v_;
  }

  const T* operator->() const noexcept {
    Read();
    return v_;
  }

  operator T() const {
    Read();
    return *v_;
  }

 private:
  void Read() const noexcept {
    // No-op
  }

 private:
  const T* v_;
};

template <typename T>
class Shared {
 public:
  template <typename... Args>
  Shared(Args&& ... a)
      : val_{std::forward<Args>(a)...} {
  }

  // Non-copyable
  Shared(const Shared&) = delete;
  Shared& operator=(const Shared&) = delete;

  // Non-movable
  Shared(Shared&&) = delete;
  Shared& operator=(Shared&&) = delete;

  ~Shared() {
    // No-op
  }

  T& operator*() noexcept {
    return val_;
  }

  const T& operator*() const noexcept {
    return val_;
  }

  operator T() const noexcept {
    return val_;
  }

  T* operator->() noexcept {
    return &val_;
  }

  const T& operator->() const noexcept {
    return &val_;
  }

  void Write(T new_val) {
    val_ = std::move(new_val);
  }

  T Read() const {
    return val_;
  }

  SharedReadView<T> ReadView() const noexcept {
    return {&val_};
  }

  SharedWriteView<T> WriteView() noexcept {
    return {&val_};
  }

 private:
  void* Loc() const noexcept {
    return (void*)&val_;
  }

 private:
  T val_;
};

}  // namespace assist

}  // namespace twist::rt::thr
